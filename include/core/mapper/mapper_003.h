#ifndef SPIKYNES_MAPPER_003_H
#define SPIKYNES_MAPPER_003_H
#include "core/mapper.h"

namespace nes_core {

    class Mapper003 final : public Mapper {
    public:
        Mapper003();

        std::uint8_t cpuRead(std::uint16_t addr) override;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) override;

        std::uint8_t ppuRead(std::uint16_t addr) override;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) override;
    };

    using MapperCNROM = Mapper003;

}

#endif //SPIKYNES_MAPPER_003_H