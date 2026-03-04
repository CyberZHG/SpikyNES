#include "core/cartridge.h"
#include "core/constants.h"

#include <fstream>
#include <ranges>
#include <algorithm>

namespace nes_core {
    Cartridge::Cartridge(Bus *bus) {
        bus_ = bus;
    }

    bool Cartridge::loadFromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return false;
        }
        const std::vector<std::uint8_t> buffer{std::istreambuf_iterator(file), std::istreambuf_iterator<char>()};
        constexpr std::array<std::uint8_t, 4> magic = {'N', 'E', 'S', 0x1A};
        const auto magicIt = std::ranges::search(buffer, magic).begin();
        if (magicIt == buffer.end()) {
            return false;
        }
        const auto headerOffset = static_cast<std::size_t>(std::distance(buffer.begin(), magicIt));
        if (headerOffset + INES_HEADER_SIZE > buffer.size()) {
            return false;
        }
        const std::uint8_t* h = buffer.data() + headerOffset;

        header_.prgRomBanks = h[4];
        header_.chrRomBanks = h[5];
        header_.verticalMirroring = h[6] & 0x01;
        header_.batteryBacked = h[6] & 0x02;
        header_.trainer = h[6] & 0x04;

        // iNES 2.0: bits 3-2 of byte 7 == 0b10
        header_.isNES2 = (h[7] & 0x0C) == 0x08;
        if (header_.isNES2) {
            header_.mapper    = static_cast<std::uint16_t>(((h[8] & 0x0F) << 8) | (h[7] & 0xF0) | (h[6] >> 4));
            header_.subMapper = (h[8] >> 4) & 0x0F;
            header_.prgRamBanks = h[10] > 0 ? h[10] : 1;
        } else {
            header_.mapper    = (h[6] >> 4) | (h[7] & 0xF0);
            header_.subMapper = 0;
            header_.prgRamBanks = h[8] > 0 ? h[8] : 1;
        }

        auto dataOffset = headerOffset + INES_HEADER_SIZE;
        if (header_.trainer) {
            dataOffset += 512;
        }

        const auto prgRomSize = header_.prgRomBanks * PRG_ROM_BANK_SIZE;
        if (dataOffset + prgRomSize > buffer.size()) {
            return false;
        }
        prgRom_.assign(buffer.begin() + dataOffset, buffer.begin() + dataOffset + prgRomSize);
        dataOffset += prgRomSize;

        const auto prgRamSize = header_.prgRamBanks * PRG_RAM_BANK_SIZE;
        prgRam_.resize(prgRamSize);

        if (header_.chrRomBanks > 0) {
            const auto chrSize = header_.chrRomBanks * CHR_ROM_BANK_SIZE;
            if (dataOffset + chrSize > buffer.size()) {
                return false;
            }
            chrRom_.assign(buffer.begin() + dataOffset, buffer.begin() + dataOffset + chrSize);
        } else {
            // iNES 2.0: byte 11 bits 3-0 encode CHR RAM size as 64 << exponent
            // iNES 1.0: default 8KB CHR RAM
            std::size_t chrRamSize = CHR_ROM_BANK_SIZE;
            if (header_.isNES2) {
                const std::uint8_t chrRamExp = h[11] & 0x0F;
                if (chrRamExp != 0) {
                    chrRamSize = 64u << chrRamExp;
                }
            }
            chrRam_.resize(chrRamSize, 0);
        }

        mirroring_ = header_.verticalMirroring ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
        initMapper(header_.mapper, header_.prgRomBanks, header_.prgRamBanks, header_.chrRomBanks);

        return true;
    }

    void Cartridge::reset() {
        if (mapper_ != nullptr) {
            mapper_->reset();
        }
    }

    void Cartridge::initPrgRom(const std::size_t size) {
        prgRom_.resize(size, 0);
    }

    void Cartridge::initPrgRam(const std::size_t size) {
        prgRam_.resize(size, 0);
    }

    void Cartridge::initChrRom(const std::size_t size) {
        chrRom_.resize(size, 0);
    }

    void Cartridge::initChrRam(const std::size_t size) {
        chrRam_.resize(size, 0);
    }

    void Cartridge::initMapper(const std::uint16_t mapperId,
        const std::uint8_t prgRomBanks, const std::uint8_t prgRamBanks, const std::uint8_t chrRomBanks) {
        mapper_ = Mapper::create(
            mapperId,
            prgRomBanks, prgRamBanks, chrRomBanks,
            &prgRom_, &prgRam_, &chrRom_, &chrRam_
        );
        if (mapper_ != nullptr) {
            if (header_.isNES2) {
                mapper_->setSubMapper(header_.subMapper);
            }
            mapper_->setBus(bus_);
        }
    }

    std::uint8_t Cartridge::cpuRead(const std::uint16_t addr) const {
        if (mapper_ != nullptr) {
            return mapper_->cpuRead(addr);
        }
        if (addr >= PRG_RAM_START && addr < PRG_ROM_START) {
            return prgRam_[addr - PRG_RAM_START];
        }
        if (prgRom_.empty()) {
            return 0;
        }
        const auto offset = (addr - PRG_ROM_START) % prgRom_.size();
        return prgRom_[offset];
    }

    void Cartridge::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (mapper_ != nullptr) {
            mapper_->cpuWrite(addr, value);
            return;
        }
        if (addr >= PRG_RAM_START && addr < PRG_ROM_START) {
            prgRam_[addr - PRG_RAM_START] = value;
            return;
        }
        if (prgRom_.empty()) {
            return;
        }
        const auto offset = (addr - PRG_ROM_START) % prgRom_.size();
        prgRom_[offset] = value;
    }

    std::uint8_t Cartridge::ppuRead(const std::uint16_t addr) const {
        if (mapper_ != nullptr) {
            return mapper_->ppuRead(addr);
        }
        if (chrRom_.empty()) {
            return 0;
        }
        if (addr < CHR_ROM_BANK_SIZE) {
            return chrRom_[addr];
        }
        return 0;
    }

    void Cartridge::notifyA12(const std::uint16_t addr) const {
        if (mapper_ != nullptr) {
            mapper_->notifyA12(addr);
        }
    }

    bool Cartridge::irqAsserted() const {
        return mapper_ != nullptr && mapper_->irqAsserted();
    }

    Mirroring Cartridge::mirroring() const {
        if (mapper_) {
            if (const auto m = mapper_->mirroring(); m != Mirroring::INVALID) {
                return m;
            }
        }
        return mirroring_;
    }

    void Cartridge::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (mapper_ != nullptr) {
            mapper_->ppuWrite(addr, value);
            return;
        }
        if (chrRom_.empty()) {
            return;
        }
        if (addr < CHR_ROM_BANK_SIZE) {
            chrRom_[addr] = value;
        }
    }
}
