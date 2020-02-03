#include "core/bus.h"
#include "core/constants.h"
#include "core/cpu.h"
#include "core/ppu.h"
#include "core/apu.h"

namespace nes_core {
    Bus::Bus(CPU* cpu, PPU* ppu, APU* apu) {
        cpu_ = cpu;
        ppu_ = ppu;
        apu_ = apu;
    }

    std::uint8_t Bus::read(const std::uint16_t addr) const {
        if (addr < CPU_RAM_END) {
            return cpu_->read(addr);
        }
        // TODO: Add other devices.
        return 0;
    }

    void Bus::write(const std::uint16_t addr, const std::uint16_t value) const {
        if (addr < CPU_RAM_END) {
            cpu_->write(addr, value);
        }
        // TODO: Add other devices.
    }
}
