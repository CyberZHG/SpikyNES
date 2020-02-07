#include "core/ppu.h"
#include "core/bus.h"

namespace nes_core {
    std::uint8_t& PPURegisters::r(const int index) {
        switch (index) {
            case 0: return ppuControl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDMA;
        }
    }

    std::uint8_t PPURegisters::r(const int index) const {
        switch (index) {
            case 0: return ppuControl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDMA;
        }
    }

    int PPURegisters::baseNametableAddr() const {
        switch (ppuControl & 0b00000011) {
            case 0: return 0x2000;
            case 1: return 0x2400;
            case 2: return 0x2800;
            default: return 0x2C00;
        }
    }

    void PPURegisters::reset() {
        ppuControl = 0;
        ppuMask = 0;
        ppuStatus = 0;
        oamAddr = 0;
        oamData = 0;
        ppuScroll = 0;
        ppuAddr = 0;
        ppuData = 0;
        oamDMA = 0;
        v = 0;
        t = 0;
        x = 0;
        w = 0;
        ppuDataBuffer = 0;
    }

    PPU::PPU(Bus *bus) {
        bus_ = bus;
    }

    void PPU::reset() {
        regs_.reset();
        oam_.fill(0);
        nametable_.fill(0);
        palette_.fill(0);
    }

    std::uint8_t PPU::registerRead(const std::uint16_t addr) {
        const auto index = addr & 0b00000111;
        auto value = regs_.r(index);
        if (index == 2) {  // PPU_STATUS
            regs_.clearVBlank();
            regs_.w = 0;
        } else if (index == 4) {  // OAM_DATA
            value = oam_[regs_.oamAddr];
        } else if (index == 7) {  // PPU_DATA
            value = regs_.ppuDataBuffer;
            regs_.ppuDataBuffer = ppuRead(regs_.v);
            if ((regs_.v & PPU_MEM_MASK) >= PPU_PALETTE_START) {
                value = regs_.ppuDataBuffer;
            }
            regs_.v += regs_.vRamIncrement();
        }
        return value;
    }

    void PPU::registerWrite(const std::uint16_t addr, const std::uint8_t value) {
        const auto index = addr & 0b00000111;
        regs_.r(index) = value;
        if (index == 0) { // PPU_CTRL
            regs_.t = (regs_.t & 0xF3FF) | ((value & 0x03) << 10);
        } else if (index == 4) {  // OAM_DATA
            oam_[regs_.oamAddr++] = value;
        } else if (index == 5) {  // PPU_SCROLL
            if (regs_.w == 0) {
                regs_.w = 1;
                regs_.t = (regs_.t & 0xFFE0) | (value >> 3);
                regs_.x = value & 0b00000111;
            } else {
                regs_.w = 0;
                regs_.t = (regs_.t & 0x8C1F) | ((value & 0x07) << 12);
                regs_.t = (regs_.t & 0xFC1F) | ((value & 0xF8) << 2);
            }
        } else if (index == 6) {  // PPU_ADDR
            if (regs_.w == 0) {
                regs_.w = 1;
                regs_.t = (regs_.t & 0x00FF) | ((value & 0x3F) << 8);
            } else {
                regs_.w = 0;
                regs_.t = (regs_.t & 0xFF00) | value;
                regs_.v = regs_.t;
            }
        } else if (index == 7) {  // PPU_DATA
            ppuWrite(regs_.v, value);
            regs_.v += regs_.vRamIncrement();
        }
    }

    std::uint16_t PPU::mirrorNametableAddr(std::uint16_t addr) const {
        switch (mirroring_) {
            case Mirroring::HORIZONTAL:
                if ((0x0400 <= addr && addr < 0x0800) || (0x0C00 <= addr && addr < 0x1000)) {
                    addr -= 0x0400;
                }
                break;
            case Mirroring::VERTICAL:
                if (0x0800 <= addr && addr < 0x1000) {
                    addr -= 0x0800;
                }
                break;
        }
        return addr;
    }

    std::uint16_t PPU::mirrorPaletteAddr(std::uint16_t addr) {
        addr &= 0x1F;
        if ((addr & 0x13) == 0x10) {
            addr &= ~0x10;
        }
        return addr;
    }

    std::uint8_t PPU::ppuRead(std::uint16_t addr) const {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            return bus_->ppuRead(addr);
        }
        if (addr < PPU_PALETTE_START) {
            if (addr >= PPU_NAMETABLE_MIRRO_START) {
                addr -= PPU_NAMETABLE_SIZE;
            }
            addr -= PPU_NAMETABLE_START;
            addr = mirrorNametableAddr(addr);
            return nametable_[addr];
        }
        return palette_[mirrorPaletteAddr(addr)];
    }

    void PPU::ppuWrite(std::uint16_t addr, const std::uint8_t value) {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            bus_->ppuWrite(addr, value);
        } else if (addr < PPU_PALETTE_START) {
            if (addr >= PPU_NAMETABLE_MIRRO_START) {
                addr -= PPU_NAMETABLE_SIZE;
            }
            addr -= PPU_NAMETABLE_START;
            addr = mirrorNametableAddr(addr);
            nametable_[addr] = value;
        } else {
            palette_[mirrorPaletteAddr(addr)] = value;
        }
    }
}
