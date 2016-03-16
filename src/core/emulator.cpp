#include "core/emulator.h"

namespace nes_core {
    Emulator::Emulator() : cpu_(&bus_), ppu_(&bus_), apu_(&bus_), bus_(&cpu_, &ppu_, &apu_, &cartridge_, &controller1_, &controller2_),
        cartridge_(&bus_) {
    }

    bool Emulator::loadROM(const std::string& path) {
        if (!cartridge_.loadFromFile(path)) {
            return false;
        }
        reset();
        return true;
    }

    void Emulator::reset() {
        cartridge_.reset();
        cpu_.reset();
        ppu_.reset();
        apu_.reset();
        controller1_.reset();
        controller2_.reset();
        availablePpuCycles_ = 0;
        ppuSubStep_ = 2;
    }

    void Emulator::run(const double milliseconds) {
        constexpr double PPU_FREQUENCY_NTSC_D = CPU_FREQUENCY_NTSC * 3.0;
        availablePpuCycles_ += PPU_FREQUENCY_NTSC_D * milliseconds / 1000.0;
        while (ppu_.ppuCycles() < availablePpuCycles_) {
            (void)step();
        }
    }

    int Emulator::step() {
        if (!cpu_.halted()) {
            ppu_.step();
        }

        bool cpuRan = false;
        if (++ppuSubStep_ == 2) {
            if (!cpu_.halted()) {
                apu_.step();
            }
            cpu_.step();
            cpuRan = true;
        }

        if (ppuSubStep_ >= 3) {
            ppuSubStep_ = 0;
            if (!cpu_.halted()) {
                cpu_.samplePrevIrqState();
            }
        }

        return cpuRan ? 1 : 0;
    }

    int Emulator::execOp() {
        const auto startCycles = cpu_.cycles();
        const auto steps = cpu_.steps();
        while (cpu_.steps() == steps) {
            step();
        }
        return static_cast<int>(cpu_.cycles() - startCycles);
    }
}
