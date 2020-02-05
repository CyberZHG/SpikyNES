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

        if (header_.trainer) {
            file.seekg(512, std::ios::cur);
        }

        const auto prgSize = header_.prgRomBanks * PRG_ROM_BANK_SIZE;
        prg_.resize(prgSize);
        file.read(reinterpret_cast<char*>(prg_.data()), prgSize);
        if (!file) {
            return false;
        }

        if (header_.chrRomBanks > 0) {
            const auto chrSize = header_.chrRomBanks * CHR_ROM_BANK_SIZE;
            chr_.resize(chrSize);
            file.read(reinterpret_cast<char*>(chr_.data()), chrSize);
        }

        return true;
    }

    void Cartridge::initPRG(const std::size_t size) {
        prg_.resize(size, 0);
    }

    std::uint8_t Cartridge::read(const std::uint16_t addr) const {
        if (prg_.empty()) {
            return 0;
        }
        const auto offset = (addr - PRG_ROM_START) % prg_.size();
        return prg_[offset];
    }

    void Cartridge::write(const std::uint16_t addr, const std::uint8_t value) {
        if (prg_.empty()) {
            return;
        }
        const auto offset = (addr - PRG_ROM_START) % prg_.size();
        prg_[offset] = value;
    }

}