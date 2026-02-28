#ifndef SPIKYNES_MAPPER_001_H
#define SPIKYNES_MAPPER_001_H

#include "core/mapper.h"

namespace nes_core {

    class Mapper001 final : public Mapper {
    public:
        Mapper001();

        void reset() override;

        std::uint8_t cpuRead(std::uint16_t addr) override;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) override;

        std::uint8_t ppuRead(std::uint16_t addr) override;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) override;

        [[nodiscard]] Mirroring mirroring() const override;

    private:
        std::uint8_t chrRomBankMode_ = 0;
        std::uint8_t prgRomBankMode_ = 3;
        std::uint8_t nametableMode_ = 0;

        std::uint8_t shiftRegister_ = 0b10000;
        std::uint8_t chrRomIndex0_ = 0;
        std::uint8_t chrRomIndex1_ = 0;

        std::int64_t lastWriteCycle_ = -2;  // for consecutive write filtering
        bool prgRamDisabled_ = false;       // MMC1B: bit 4 of $E000

        void writeRegister(std::uint16_t addr, std::uint8_t value);
    };

    using MapperMMC1 = Mapper001;

}

#endif //SPIKYNES_MAPPER_001_H