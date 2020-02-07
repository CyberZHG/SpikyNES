#ifndef SPIKYNES_CARTRIDGE_H
#define SPIKYNES_CARTRIDGE_H

#include <cstdint>
#include <string>
#include <vector>

namespace nes_core {

    struct INESHeader {
        std::uint8_t prgRomBanks = 0;  // PRG ROM size in 16KB units
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
        void initPRG(std::size_t size);
        /** Initialize PRG ROM with given size (for testing). */
        void initCHR(std::size_t size);

        std::uint8_t cpuRead(std::uint16_t addr) const;
        void cpuWrite(std::uint16_t addr, std::uint8_t value);

        std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value);

        const INESHeader& header() const { return header_; }

    private:
        INESHeader header_;
        std::vector<std::uint8_t> prg_;
        std::vector<std::uint8_t> chr_;
    };

}

#endif //SPIKYNES_CARTRIDGE_H
