#ifndef SPIKYNES_PPU_H
#define SPIKYNES_PPU_H

#include <array>

#include "core/constants.h"

namespace nes_core {

    class Bus;

    class PPU {
    public:
        explicit PPU(Bus* bus);

        void reset();

        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

    private:
        Bus* bus_ = nullptr;
        std::array<std::uint8_t, PPU_REG_COUNT> regs_ = {};
    };

}

#endif //SPIKYNES_PPU_H