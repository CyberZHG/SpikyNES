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

    void Bus::reset() {
        lastBusValue_ = 0;
    }

    std::uint8_t Bus::cpuRead(const std::uint16_t addr) {
        std::uint8_t value = lastBusValue_;
        if (addr < CPU_RAM_END) {
            value = cpu_->cpuRead(addr);
        } else if (addr < PPU_REG_END) {
            value = ppu_->registerRead(addr);
        } else if (addr == 0x4016) {
            value = controller1_->read();
        } else if (addr == 0x4017) {
            value = controller2_->read();
        } else if (addr < APU_REG_END) {
            value = apu_->read(addr);
        } else if (addr >= PRG_RAM_START) {
            value = cartridge_->cpuRead(addr);
        }
        return lastBusValue_ = value;
    }

    void Bus::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
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
        lastBusValue_ = value;
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

    void Bus::cpuIRQ() const {
        cpu_->irq();
    }

    void Bus::cpuStall(const int cycles) const {
        cpu_->stall(cycles);
    }
}
