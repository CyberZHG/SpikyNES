#include "core/bus.h"
#include "core/constants.h"
#include "core/cpu.h"
#include "core/ppu.h"
#include "core/apu.h"
#include "core/cartridge.h"
#include "core/controller.h"

namespace nes_core {
    Bus::Bus(CPU* cpu, PPU* ppu, APU* apu, Cartridge* cartridge, Controller* controller1, Controller* controller2) {
        cpu_ = cpu;
        ppu_ = ppu;
        apu_ = apu;
        cartridge_ = cartridge;
        controller1_ = controller1;
        controller2_ = controller2;
    }

    std::uint8_t Bus::cpuRead(const std::uint16_t addr) const {
        if (addr < CPU_RAM_END) {
            return cpu_->cpuRead(addr);
        }
        if (addr < PPU_REG_END) {
            return ppu_->registerRead(addr);
        }
        if (addr == 0x4016) {
            return controller1_->read();
        }
        if (addr == 0x4017) {
            return controller2_->read();
        }
        if (addr < APU_REG_END) {
            return apu_->read(addr);
        }
        if (addr >= PRG_RAM_START) {
            return cartridge_->cpuRead(addr);
        }
        return 0;
    }

    void Bus::cpuWrite(const std::uint16_t addr, const std::uint8_t value) const {
        if (addr < CPU_RAM_END) {
            cpu_->cpuWrite(addr, value);
        } else if (addr < PPU_REG_END) {
            ppu_->registerWrite(addr, value);
        } else if (addr == 0x4014) {  // OAM DMA
            cpu_->triggerDMA(value);
        } else if (addr == 0x4016) {
            controller1_->write(value);
            controller2_->write(value);
        } else if (addr < APU_REG_END) {
            apu_->write(addr, value);
        } else if (addr >= PRG_RAM_START) {
            cartridge_->cpuWrite(addr, value);
        }
    }

    std::uint8_t Bus::ppuRead(std::uint16_t addr) const {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            return cartridge_->ppuRead(addr);
        }
        return 0;
    }

    void Bus::ppuWrite(std::uint16_t addr, const std::uint8_t value) const {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            cartridge_->ppuWrite(addr, value);
        }
    }

    void Bus::cpuNMI() const {
        cpu_->nmi();
    }

}
