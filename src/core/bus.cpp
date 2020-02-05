#include "core/bus.h"
#include "core/constants.h"
#include "core/cpu.h"
#include "core/ppu.h"
#include "core/apu.h"
#include "core/cartridge.h"

namespace nes_core {
    Bus::Bus(CPU* cpu, PPU* ppu, APU* apu, Cartridge* cartridge) {
        cpu_ = cpu;
        ppu_ = ppu;
        apu_ = apu;
        cartridge_ = cartridge;
    }

    std::uint8_t Bus::read(const std::uint16_t addr) const {
        if (addr < CPU_RAM_END) {
            return cpu_->read(addr);
        }
        if (addr < PPU_REG_END) {
            return ppu_->read(addr);
        }
        if (addr < APU_REG_END) {
            return apu_->read(addr);
        }
        if (addr >= PRG_ROM_START) {
            return cartridge_->read(addr);
        }
        return 0;
    }

    void Bus::write(const std::uint16_t addr, const std::uint8_t value) const {
        if (addr < CPU_RAM_END) {
            cpu_->write(addr, value);
        } else if (addr < PPU_REG_END) {
            ppu_->write(addr, value);
        } else if (addr < APU_REG_END) {
            apu_->write(addr, value);
        } else if (addr >= PRG_ROM_START) {
            cartridge_->write(addr, value);
        }
    }
}
