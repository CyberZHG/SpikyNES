#include "core/ppu.h"

namespace nes_core {
    std::uint8_t& PPURegisters::r(const int index) {
        switch (index) {
            case 0: return ppuCtrl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDma;
        }
    }

    std::uint8_t PPURegisters::r(const int index) const {
        switch (index) {
            case 0: return ppuCtrl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDma;
        }
    }

    int PPURegisters::baseNametableAddr() const {
        switch (ppuCtrl & 0b00000011) {
            case 0: return 0x2000;
            case 1: return 0x2400;
            case 2: return 0x2800;
            default: return 0x2C00;
        }
    }

    PPU::PPU(Bus *bus) {
        bus_ = bus;
    }

    void PPU::reset() {
    }

    std::uint8_t PPU::read(const std::uint16_t addr) {
        const auto index = addr & 0b00000111;
        const auto value = regs_.r(index);
        if (index == 2) {
            regs_.clearVBlank();
            regs_.w = 0;
        }
        return value;
    }

    void PPU::write(const std::uint16_t addr, const std::uint8_t value) {
        const auto index = addr & 0b00000111;
        regs_.r(index) = value;
        if (index == 0) {
            regs_.t = (regs_.t & 0xF3FF) | ((value & 0x03) << 10);
        }
    }
}
