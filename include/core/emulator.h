#ifndef SPIKYNES_CORE_H
#define SPIKYNES_CORE_H

#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "cartridge.h"
#include "controller.h"

#include <memory>
#include <string>

namespace nes_core {

    class Emulator {
    public:
        Emulator();

        CPU& cpu() { return cpu_; }
        [[nodiscard]] const CPU& cpu() const { return cpu_; }
        PPU& ppu() { return ppu_; }
        [[nodiscard]] const PPU& ppu() const { return ppu_; }
        APU& apu() { return apu_; }
        [[nodiscard]] const APU& apu() const { return apu_; }
        Bus& bus() { return bus_; }
        [[nodiscard]] const Bus& bus() const { return bus_; }
       Cartridge& cartridge() { return cartridge_; }
        [[nodiscard]] const Cartridge& cartridge() const { return cartridge_; }
        Controller& controller1() { return controller1_; }
        [[nodiscard]] const Controller& controller1() const { return controller1_; }
        Controller& controller2() { return controller2_; }
        [[nodiscard]] const Controller& controller2() const { return controller2_; }

        bool loadROM(const std::string& path);
        void reset();

        void run(double milliseconds);

        int step();
        int execOp();

    private:
        CPU cpu_;
        PPU ppu_;
        APU apu_;
        Bus bus_;
        Cartridge cartridge_;
        Controller controller1_;
        Controller controller2_;

        double availablePpuCycles_ = 0;
        int ppuSubStep_ = 0;  // 0,1,2 — CPU fires when ppuSubStep_ reaches 2 (P-CPU-PP order)
    };

}

#endif //SPIKYNES_CORE_H