#ifndef SPIKYNES_PPU_H
#define SPIKYNES_PPU_H

#include <array>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct PPURegisters {
        // MMIO registers;
        std::uint8_t ppuCtrl;
        std::uint8_t ppuMask;
        std::uint8_t ppuStatus;
        std::uint8_t oamAddr;
        std::uint8_t oamData;
        std::uint8_t ppuScroll;
        std::uint8_t ppuAddr;
        std::uint8_t ppuData;
        std::uint8_t oamDma;

        // Internal registers
        std::uint16_t v;
        std::uint16_t t;
        std::uint16_t x;
        std::uint16_t w;

        std::uint8_t& r(int index);
        std::uint8_t r(int index) const;

        // PPUCTRL - Miscellaneous settings ($2000 write)
        bool vBlankNmiEnabled() const { return getBit(ppuCtrl, 7); }
        int spriteSize() const { return getBit(ppuCtrl, 5) ? 16 : 8; }
        int spritePatternTableAddr() const { return getBit(ppuCtrl, 4) ? 0x1000 : 0x0000; }
        int vRamIncrement() const { return getBit(ppuCtrl, 3) ? 32 : 1; }
        int baseNametableAddr() const;

        // PPUMASK - Rendering settings ($2001 write)
        bool emphasizeBlue() const { return getBit(ppuMask, 7); }
        bool emphasizeGreen() const { return getBit(ppuMask, 6); }
        bool emphasizeRed() const { return getBit(ppuMask, 5); }
        bool spriteRenderingEnabled() const { return getBit(ppuMask, 4); }
        bool backgroundRenderingEnabled() const { return getBit(ppuMask, 3); }
        bool renderingEnabled() const { return ppuMask & 0b00011000; }
        bool showSpritesInLeft() const { return getBit(ppuMask, 2); }
        bool showBackgroundInLeft() const { return getBit(ppuMask, 1); }
        bool grayscale() const { return getBit(ppuMask, 0); }
    };

    class PPU {
    public:
        explicit PPU(Bus* bus);

        void reset();

        PPURegisters& registers() { return regs_; }
        const PPURegisters& registers() const { return regs_; }

        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

    private:
        Bus* bus_ = nullptr;
        PPURegisters regs_{};
    };

}

#endif //SPIKYNES_PPU_H