#ifndef SPIKYNES_MAPPER_000_H
#define SPIKYNES_MAPPER_000_H
#include "core/mapper.h"

namespace nes_core {

    class Mapper000 final : public Mapper {
    public:
        std::uint8_t cpuRead(std::uint16_t addr) override;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) override;

        std::uint8_t ppuRead(std::uint16_t addr) override;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) override;
    };

    using MapperNROM = Mapper000;

}

#endif //SPIKYNES_MAPPER_000_H