#ifndef SPIKYNES_BUS_H
#define SPIKYNES_BUS_H

#include <cstdint>

namespace nes_core {

    class CPU;
    class PPU;
    class APU;

    class Bus {
    public:
        Bus(CPU* cpu, PPU* ppu, APU* apu);

        /** Read a byte from a device. */
        std::uint8_t read(std::uint16_t addr) const;
        /** Set a byte to a device. */
        void write(std::uint16_t addr, std::uint16_t value) const;

    private:
        CPU* cpu_ = nullptr;
        PPU* ppu_ = nullptr;
        APU* apu_ = nullptr;
    };

}

#endif //SPIKYNES_BUS_H