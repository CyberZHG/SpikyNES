#ifndef SPIKYNES_BUS_H
#define SPIKYNES_BUS_H

#include <cstdint>

#include "core/constants.h"

namespace nes_core {

    class CPU;
    class PPU;
    class APU;
    class Cartridge;
    class Controller;

    class Bus {
    public:
        Bus(CPU* cpu, PPU* ppu, APU* apu, Cartridge* cartridge, Controller* controller1, Controller* controller2);

        void reset();

        [[nodiscard]] std::uint8_t cpuRead(std::uint16_t addr);
        void cpuWrite(std::uint16_t addr, std::uint8_t value);
        [[nodiscard]] std::uint8_t lastBusValue() const { return lastBusValue_; }
        void setLastBusValue(const std::uint8_t value) { lastBusValue_ = value; }

        [[nodiscard]] std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) const;

        void cpuNMI() const;
        void cpuNMIClear() const;
        void cpuStall(int cycles) const;
        [[nodiscard]] std::int64_t cpuCycles() const;

        void notifyA12(std::uint16_t addr) const;

        [[nodiscard]] bool irqAsserted() const;
        [[nodiscard]] Mirroring mirroring() const;

    private:
        CPU* cpu_ = nullptr;
        PPU* ppu_ = nullptr;
        APU* apu_ = nullptr;
        Cartridge* cartridge_ = nullptr;
        Controller* controller1_ = nullptr;
        Controller* controller2_ = nullptr;

        std::uint8_t lastBusValue_ = 0;
    };

}

#endif //SPIKYNES_BUS_H