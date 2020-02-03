#ifndef SPIKYNES_CORE_H
#define SPIKYNES_CORE_H

#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "cartridge.h"

#include <memory>

namespace nes_core {

    class Emulator {
    public:
        Emulator() = default;
        virtual ~Emulator() = default;

        // void run(double milliseconds);

    private:
        CPU cpu_;
        PPU ppu_;
        APU apu_;
        Bus bus_;
        Cartridge cartridge_;
    };

}

#endif //SPIKYNES_CORE_H