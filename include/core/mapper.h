#ifndef SPIKYNES_MAPPER_H
#define SPIKYNES_MAPPER_H

#include <cstdint>
#include <vector>
#include <memory>

#include "core/constants.h"

namespace nes_core {
    class Bus;

    class Mapper {
    public:
        Mapper() = default;
        virtual ~Mapper() = default;

        [[nodiscard]] static std::unique_ptr<Mapper> create(std::uint16_t mapperId,
            std::uint8_t prgRomBanks, std::uint8_t prgRamBanks, std::uint8_t chrRomBanks,
            std::vector<std::uint8_t>* prgRom, std::vector<std::uint8_t>* prgRam, std::vector<std::uint8_t>* chrRom);

        [[nodiscard]] virtual std::uint8_t cpuRead(std::uint16_t addr) = 0;
        virtual void cpuWrite(std::uint16_t addr, std::uint8_t value) = 0;

        [[nodiscard]] virtual std::uint8_t ppuRead(std::uint16_t addr) = 0;
        virtual void ppuWrite(std::uint16_t addr, std::uint8_t value) = 0;

        [[nodiscard]] virtual Mirroring mirroring() const { return Mirroring::INVALID; }

        void setSubMapper(const std::uint8_t subMapperId) { subMapper_ = subMapperId; };
        void setBus(Bus* bus) { bus_ = bus; }

    protected:
        Bus* bus_ = nullptr;

        std::uint8_t prgRomBanks_ = 0;
        std::uint8_t prgRamBanks_ = 0;
        std::uint8_t chrRomBanks_ = 0;
        std::vector<std::uint8_t>* prgRom_ = nullptr;
        std::vector<std::uint8_t>* prgRam_ = nullptr;
        std::vector<std::uint8_t>* chrRom_ = nullptr;
        std::uint8_t prgRomIndex_ = 0;
        std::uint8_t prgRamIndex_ = 0;
        std::uint8_t chrRomIndex_ = 0;

        std::uint8_t subMapper_ = 0;
    };

}

#endif //SPIKYNES_MAPPER_H