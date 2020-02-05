#ifndef SPIKYNES_APU_H
#define SPIKYNES_APU_H

#include <array>

#include "core/constants.h"

namespace nes_core {

    class Bus;

    class APU {
    public:
        explicit APU(Bus* bus);

        void reset();

        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

    private:
        Bus* bus_ = nullptr;
        std::array<std::uint8_t, APU_REG_COUNT> regs_ = {};
    };

}

#endif //SPIKYNES_APU_H