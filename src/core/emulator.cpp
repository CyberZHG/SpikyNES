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
        availableCycles_ = 0;
    }

    void Emulator::run(const double milliseconds) {
        availableCycles_ += CPU_FREQUENCY_NTSC * milliseconds / 1000.0;
        while (cpu_.cycles() < availableCycles_) {
            step();
        }
    }

    int Emulator::step() {
        const auto cycles = cpu_.step();
        for (int i = 0; i < cycles * 3; ++i) {
            ppu_.step();
        }
        for (int i = 0; i < cycles; ++i) {
            apu_.step();
        }
        return cycles;
    }
}
