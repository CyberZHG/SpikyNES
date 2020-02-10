#ifndef SPIKYNES_PPU_H
#define SPIKYNES_PPU_H

#include <array>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct PPURegisters {
        // MMIO registers
        std::uint8_t ppuControl; // $2000: VPHB SINN, write
        std::uint8_t ppuMask;    // $2001: BGRs bMmG, write
        std::uint8_t ppuStatus;  // $2002: VSO- ----, read
        std::uint8_t oamAddr;    // $2003: AAAA AAAA, write
        std::uint8_t oamData;    // $2004: DDDD DDDD, read/write
        std::uint8_t ppuScroll;  // $2005: XXXX XXXX, YYYY YYYY, write x2
        std::uint8_t ppuAddr;    // $2006: ..AA AAAA, AAAA AAAA,  write x2
        std::uint8_t ppuData;    // $2007: DDDD DDDD, read/write
        std::uint8_t oamDMA;     // $4014: AAAA AAAA, write

        /**
         * Internal registers
         *
         * v and t (15 bits):
         *   yyy NN YYYYY XXXXX
         *   ||| || ||||| +++++-- coarse X scroll (0-4)
         *   ||| || +++++-------- coarse Y scroll (5-9)
         *   ||| ++-------------- nametable select (10-11)
         *   +++----------------- fine Y scroll (12-14)
         */
        std::uint16_t v;  // Current VRAM address
        std::uint16_t t;  // Temporary VRAM address
        std::uint8_t x;   // Fine X scroll (3 bits)
        std::uint8_t w;   // Write toggle (1 bit)

        // Shift registers
        std::uint16_t bgPatternShiftLo;
        std::uint16_t bgPatternShiftHi;
        std::uint16_t bgAttrShiftLo;
        std::uint16_t bgAttrShiftHi;

        std::uint8_t ppuDataBuffer;

        void reset();

        std::uint8_t& r(int index);
        std::uint8_t r(int index) const;

        // PPU_CTRL - Miscellaneous settings ($2000 write)
        bool vBlankNmiEnabled() const { return getBit(ppuControl, 7); }
        int spriteSize() const { return getBit(ppuControl, 5) ? 16 : 8; }
        int backgroundPatternTableAddr() const { return getBit(ppuControl, 4) ? 0x1000 : 0x0000; }
        int spritePatternTableAddr() const { return getBit(ppuControl, 3) ? 0x1000 : 0x0000; }
        int vRamIncrement() const { return getBit(ppuControl, 2) ? 32 : 1; }
        int baseNametableAddr() const;

        // PPU_MASK - Rendering settings ($2001 write)
        bool emphasizeBlue() const { return getBit(ppuMask, 7); }
        bool emphasizeGreen() const { return getBit(ppuMask, 6); }
        bool emphasizeRed() const { return getBit(ppuMask, 5); }
        bool spriteRenderingEnabled() const { return getBit(ppuMask, 4); }
        bool backgroundRenderingEnabled() const { return getBit(ppuMask, 3); }
        bool renderingEnabled() const { return ppuMask & 0b00011000; }
        bool showSpritesInLeft() const { return getBit(ppuMask, 2); }
        bool showBackgroundInLeft() const { return getBit(ppuMask, 1); }
        bool grayscale() const { return getBit(ppuMask, 0); }

        // PPU_STATUS - Rendering events ($2002 read)
        bool vBlank() const { return getBit(ppuStatus, 7); }
        void setVBlank() { setBit(ppuStatus, 7); }
        void clearVBlank() { clearBit(ppuStatus, 7); }
        bool sprite0Hit() const { return getBit(ppuStatus, 6); }
        bool spriteOverflow() const { return getBit(ppuStatus, 5); }
    };

    class PPU {
    public:
        explicit PPU(Bus* bus);

        void reset();

        PPURegisters& registers() { return regs_; }
        const PPURegisters& registers() const { return regs_; }

        std::uint8_t registerRead(std::uint16_t addr);
        void registerWrite(std::uint16_t addr, std::uint8_t value);

        std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value);

        void step();

        enum class Mirroring {
            HORIZONTAL,
            VERTICAL,
        };
        void setMirroring(const Mirroring mirroring) { mirroring_ = mirroring; }

        std::int32_t scanline() const { return scanline_; }
        std::int32_t cycle() const { return cycle_; }
        std::int64_t frame() const { return frame_; }
        const std::array<std::uint32_t, PPU_FRAME_BUFFER_SIZE>& frameBuffer() const { return frameBuffer_; }

    protected:
        std::uint16_t mirrorNametableAddr(std::uint16_t addr) const;
        static std::uint16_t mirrorPaletteAddr(std::uint16_t addr);

        void fetchNametableByte();
        void fetchAttributeTableByte();
        void fetchPatternTableTileLo();
        void fetchPatternTableTileHi();

        void updateShiftRegisters();
        void incrementScrollX();
        void incrementScrollY();
        void copyScrollX();
        void copyScrollY();

        void fetchTileData();
        void renderPixel();

    private:
        Bus* bus_ = nullptr;
        PPURegisters regs_{};

        std::array<std::uint8_t, PPU_OAM_SIZE> oam_{};
        std::array<std::uint8_t, PPU_NAMETABLE_SIZE> nametable_{};
        std::array<std::uint8_t, PPU_PALETTE_SIZE> palette_{};
        std::array<std::uint32_t, PPU_FRAME_BUFFER_SIZE> frameBuffer_{};

        Mirroring mirroring_ = Mirroring::HORIZONTAL;
        std::int32_t scanline_ = 0;
        std::int32_t cycle_ = 0;
        std::int64_t frame_ = 1;
        bool suppressVBlank_ = false;

        std::uint8_t nametableByte_ = 0;
        std::uint8_t attrTableByte_ = 0;
        std::uint8_t patternTableTileLo_ = 0;
        std::uint8_t patternTableTileHi_ = 0;
    };

}

#endif //SPIKYNES_PPU_H