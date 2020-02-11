#include "core/cartridge.h"
#include "core/constants.h"

#include <fstream>

namespace nes_core {

    bool Cartridge::loadFromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return false;
        }
        std::array<std::uint8_t, INES_HEADER_SIZE> headerBytes{};
        file.read(reinterpret_cast<char*>(headerBytes.data()), INES_HEADER_SIZE);
        if (!file) {
            return false;
        }
        if (headerBytes[0] != 'N' || headerBytes[1] != 'E' || headerBytes[2] != 'S' || headerBytes[3] != 0x1A) {
            return false;
        }
        header_.prgRomBanks = headerBytes[4];
        header_.chrRomBanks = headerBytes[5];
        header_.mapper = (headerBytes[6] >> 4) | (headerBytes[7] & 0xF0);
        header_.verticalMirroring = headerBytes[6] & 0x01;
        header_.batteryBacked = headerBytes[6] & 0x02;
        header_.trainer = headerBytes[6] & 0x04;
        header_.prgRamBanks = headerBytes[8] > 0 ? headerBytes[8] : 1;

        if (header_.trainer) {
            file.seekg(512, std::ios::cur);
        }

        const auto prgRomSize = header_.prgRomBanks * PRG_ROM_BANK_SIZE;
        prgRom_.resize(prgRomSize);
        file.read(reinterpret_cast<char*>(prgRom_.data()), static_cast<std::streamsize>(prgRomSize));
        if (!file) {
            return false;
        }

        const auto prgRamSize = header_.prgRamBanks * PRG_RAM_BANK_SIZE;
        prgRam_.resize(prgRamSize);

        if (header_.chrRomBanks > 0) {
            const auto chrSize = header_.chrRomBanks * CHR_ROM_BANK_SIZE;
            chrRom_.resize(chrSize);
            file.read(reinterpret_cast<char*>(chrRom_.data()), static_cast<std::streamsize>(chrSize));
        } else {
            chrRom_.resize(CHR_ROM_BANK_SIZE, 0);
        }

        initMapper(header_.mapper, header_.prgRomBanks, header_.prgRamBanks, header_.chrRomBanks);

        return true;
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

    void Cartridge::initMapper(const std::uint16_t mapperId,
        const std::uint8_t prgRomBanks, const std::uint8_t prgRamBanks, const std::uint8_t chrRomBanks) {
        mapper_ = Mapper::create(
            mapperId,
            prgRomBanks, prgRamBanks, chrRomBanks,
            &prgRom_, &prgRam_, &chrRom_
        );
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
