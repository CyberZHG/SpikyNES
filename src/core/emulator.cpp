#include "core/emulator.h"

namespace nes_core {
    Emulator::Emulator() : cpu_(&bus_), ppu_(&bus_), apu_(&bus_), bus_(&cpu_, &ppu_, &apu_, &cartridge_, &controller1_, &controller2_) {
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
        controller1_.reset();
        controller2_.reset();
        availableCycles_ = 0;
    }

    void Emulator::run(const double milliseconds) {
        availableCycles_ += CPU_FREQUENCY_NTSC * milliseconds / 1000.0;
        while (cpu_.cycles() < availableCycles_) {
            (void)step();
        }
    }

    int Emulator::step() {
        cpu_.step();
        if (!cpu_.halted()) {
            for (int i = 0; i < 3; ++i) {
                ppu_.step();
            }
            apu_.step();
        }
        return 1;
    }

    int Emulator::execOp() {
        const auto steps = cpu_.steps();
        std::int16_t cycles = 0;
        while (cpu_.steps() == steps) {
            step();
            ++cycles;
        }
        return cycles;
    }
}
