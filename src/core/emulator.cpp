#include "core/emulator.h"

namespace nes_core {
    Emulator::Emulator() : cpu_(&bus_), ppu_(&bus_), apu_(&bus_), bus_(&cpu_, &ppu_, &apu_, &cartridge_) {
    }

    bool Emulator::loadROM(const std::string& path) {
        if (!cartridge_.loadFromFile(path)) {
            return false;
        }
        ppu_.setMirroring(cartridge_.header().verticalMirroring ? PPU::Mirroring::VERTICAL : PPU::Mirroring::HORIZONTAL);
        reset();
        return true;
    }

    void Emulator::reset() {
        cpu_.reset();
        ppu_.reset();
        apu_.reset();
    }
}
