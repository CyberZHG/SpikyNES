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

        /** Read a byte from a device. */
        std::uint8_t read(std::uint16_t addr) const;
        /** Set a byte to a device. */
        void write(std::uint16_t addr, std::uint8_t value) const;

    private:
        CPU* cpu_ = nullptr;
        PPU* ppu_ = nullptr;
        APU* apu_ = nullptr;
        Cartridge* cartridge_ = nullptr;
    };

}

#endif //SPIKYNES_BUS_H