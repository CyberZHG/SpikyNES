#ifndef SPIKYNES_CARTRIDGE_H
#define SPIKYNES_CARTRIDGE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "constants.h"
#include "core/mapper.h"

namespace nes_core {

    struct INESHeader {
        std::uint8_t prgRomBanks = 0;  // PRG ROM size in 16KB units
        std::uint8_t prgRamBanks = 0;  // PRG RAM size in 8KB units
        std::uint8_t chrRomBanks = 0;  // CHR ROM size in 8KB units
        std::uint8_t mapper = 0;
        bool verticalMirroring = false;
        bool batteryBacked = false;
        bool trainer = false;
    };

    class Cartridge {
    public:
        bool loadFromFile(const std::string& path);

        /** Initialize PRG ROM with given size (for testing). */
        void initPrgRom(std::size_t size);
        /** Initialize PRG RAM with given size (for testing). */
        void initPrgRam(std::size_t size = PRG_RAM_BANK_SIZE);
        /** Initialize CHR ROM with given size (for testing). */
        void initChrRom(std::size_t size);

        void initMapper(std::uint16_t mapperId, std::uint8_t prgRomBanks, std::uint8_t prgRamBanks, std::uint8_t chrRomBanks);

        [[nodiscard]] std::uint8_t cpuRead(std::uint16_t addr) const;
        void cpuWrite(std::uint16_t addr, std::uint8_t value);

        [[nodiscard]] std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value);

        [[nodiscard]] const INESHeader& header() const { return header_; }

    private:
        INESHeader header_{};
        std::vector<std::uint8_t> prgRom_{};
        std::vector<std::uint8_t> prgRam_{};
        std::vector<std::uint8_t> chrRom_{};
        std::unique_ptr<Mapper> mapper_ = nullptr;
    };

}

#endif //SPIKYNES_CARTRIDGE_H
