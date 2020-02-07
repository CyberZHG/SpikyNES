#ifndef SPIKYNES_BUS_H
#define SPIKYNES_BUS_H

#include <cstdint>

namespace nes_core {

    class CPU;
    class PPU;
    class APU;
    class Cartridge;

    class Bus {
    public:
        Bus(CPU* cpu, PPU* ppu, APU* apu, Cartridge* cartridge);

        std::uint8_t cpuRead(std::uint16_t addr) const;
        void cpuWrite(std::uint16_t addr, std::uint8_t value) const;

        std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value) const;

    private:
        CPU* cpu_ = nullptr;
        PPU* ppu_ = nullptr;
        APU* apu_ = nullptr;
        Cartridge* cartridge_ = nullptr;
    };

}

#endif //SPIKYNES_BUS_H