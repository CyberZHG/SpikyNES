#include "core/emulator.h"

namespace nes_core {
    Emulator::Emulator() : cpu_(&bus_), ppu_(&bus_), apu_(&bus_), bus_(&cpu_, &ppu_, &apu_) {
    }

    void Emulator::reset() {
        cpu_.reset();
        ppu_.reset();
        apu_.reset();
    }
}
