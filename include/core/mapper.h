#ifndef SPIKYNES_MAPPER_H
#define SPIKYNES_MAPPER_H

#include <cstdint>
#include <vector>
#include <memory>

namespace nes_core {
    class Mapper {
    public:
        Mapper() = default;
        virtual ~Mapper() = default;

        static std::unique_ptr<Mapper> create(std::uint16_t mapperId,
            std::uint8_t prgRomBanks, std::uint8_t prgRamBanks, std::uint8_t chrRomBanks,
            std::vector<std::uint8_t>* prgRom, std::vector<std::uint8_t>* prgRam, std::vector<std::uint8_t>* chrRom);

        virtual std::uint8_t cpuRead(std::uint16_t addr) = 0;
        virtual void cpuWrite(std::uint16_t addr, std::uint8_t value) = 0;

        virtual std::uint8_t ppuRead(std::uint16_t addr) = 0;
        virtual void ppuWrite(std::uint16_t addr, std::uint8_t value) = 0;

    protected:
        std::uint8_t prgRomBanks_ = 0;
        std::uint8_t prgRamBanks_ = 0;
        std::uint8_t chrRomBanks_ = 0;
        std::vector<std::uint8_t>* prgRom_ = nullptr;
        std::vector<std::uint8_t>* prgRam_ = nullptr;
        std::vector<std::uint8_t>* chrRom_ = nullptr;
    };

}

#endif //SPIKYNES_MAPPER_H