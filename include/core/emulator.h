#ifndef SPIKYNES_CORE_H
#define SPIKYNES_CORE_H

#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "cartridge.h"

#include <memory>
#include <string>

namespace nes_core {

    class Emulator {
    public:
        Emulator();
        virtual ~Emulator() = default;

        CPU& cpu() { return cpu_; }
        const CPU& cpu() const { return cpu_; }
        PPU& ppu() { return ppu_; }
        const PPU& ppu() const { return ppu_; }
        APU& apu() { return apu_; }
        const APU& apu() const { return apu_; }
        Bus& bus() { return bus_; }
        const Bus& bus() const { return bus_; }
        Cartridge& cartridge() { return cartridge_; }
        const Cartridge& cartridge() const { return cartridge_; }

        bool loadROM(const std::string& path);
        void reset();

        void run(double milliseconds);

        int step();

    private:
        CPU cpu_;
        PPU ppu_;
        APU apu_;
        Bus bus_;
        Cartridge cartridge_;

        int availableCycles_ = 0;
    };

}

#endif //SPIKYNES_CORE_H