#ifndef SPIKYNES_CARTRIDGE_H
#define SPIKYNES_CARTRIDGE_H

#include <array>
#include "core/constants.h"

namespace nes_core {

    class Cartridge {
    public:
        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

    private:
        std::array<std::uint8_t, PRG_ROM_SIZE> prg_ = {};
    };

}

#endif //SPIKYNES_CARTRIDGE_H