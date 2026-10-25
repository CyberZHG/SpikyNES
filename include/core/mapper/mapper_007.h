#ifndef SPIKYNES_MAPPER_007_H
#define SPIKYNES_MAPPER_007_H
#include "core/mapper.h"

namespace nes_core {

    class Mapper007 final : public Mapper {
    public:
        std::uint8_t cpuRead(std::uint16_t addr) override;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) override;

        std::uint8_t ppuRead(std::uint16_t addr) override;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) override;

        [[nodiscard]] Mirroring mirroring() const override;

    private:
        std::uint8_t nametableMode_ = 0;
    };

    using MapperAxROM = Mapper007;

}

#endif //SPIKYNES_MAPPER_007_H