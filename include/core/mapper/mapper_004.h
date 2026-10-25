#ifndef SPIKYNES_MAPPER_004_H
#define SPIKYNES_MAPPER_004_H

#include <array>
#include "core/mapper.h"

namespace nes_core {

    class Mapper004 final : public Mapper {
    public:
        Mapper004();

        void reset() override;

        std::uint8_t cpuRead(std::uint16_t addr) override;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) override;

        std::uint8_t ppuRead(std::uint16_t addr) override;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) override;

        [[nodiscard]] Mirroring mirroring() const override;
        [[nodiscard]] bool irqAsserted() const override;

        void notifyA12(std::uint16_t addr) override;
        void ppuStep() override;

    private:
        [[nodiscard]] int chrBankAddr(std::uint16_t addr) const;
        void clockIrqCounter();

        std::uint8_t bankToUpdate_ = 0;
        std::uint8_t prgRomBankMode_ = 0;
        std::uint8_t chrRomBankMode_ = 0;
        std::uint8_t nametableMode_ = 0;

        std::array<std::uint8_t, 8> romIndices_{};

        bool writeProtected_ = false;
        bool prgRamEnabled_ = false;

        std::uint8_t irqLatch_ = 0;
        std::uint8_t irqCounter_ = 0;
        bool irqDisabled_ = false;
        bool irqReload_ = false;
        bool irqFlag_ = false;

        std::int64_t a12LowCycle_ = 0;
        std::int32_t irqDelay_ = 0;
    };

    using MapperMMC3 = Mapper004;

}

#endif //SPIKYNES_MAPPER_004_H