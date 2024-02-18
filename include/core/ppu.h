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
        [[nodiscard]] std::uint8_t r(int index) const;

        // PPU_CTRL - Miscellaneous settings ($2000 write)
        [[nodiscard]] bool vBlankNmiEnabled() const { return getBit(ppuControl, 7); }
        [[nodiscard]] int spriteSize() const { return getBit(ppuControl, 5) ? 16 : 8; }
        [[nodiscard]] int backgroundPatternTableAddr() const { return getBit(ppuControl, 4) ? 0x1000 : 0x0000; }
        [[nodiscard]] int spritePatternTableAddr() const { return getBit(ppuControl, 3) ? 0x1000 : 0x0000; }
        [[nodiscard]] int vRamIncrement() const { return getBit(ppuControl, 2) ? 32 : 1; }
        [[nodiscard]] int baseNametableAddr() const;

        // PPU_MASK - Rendering settings ($2001 write)
        [[nodiscard]] bool emphasizeBlue() const { return getBit(ppuMask, 7); }
        [[nodiscard]] bool emphasizeGreen() const { return getBit(ppuMask, 6); }
        [[nodiscard]] bool emphasizeRed() const { return getBit(ppuMask, 5); }
        [[nodiscard]] bool spriteRenderingEnabled() const { return getBit(ppuMask, 4); }
        [[nodiscard]] bool backgroundRenderingEnabled() const { return getBit(ppuMask, 3); }
        [[nodiscard]] bool renderingEnabled() const { return ppuMask & 0b00011000; }
        [[nodiscard]] bool bothRenderingEnabled() const { return (ppuMask & 0b00011000) == 0b00011000; }
        [[nodiscard]] bool showSpritesInLeft() const { return getBit(ppuMask, 2); }
        [[nodiscard]] bool showBackgroundInLeft() const { return getBit(ppuMask, 1); }
        [[nodiscard]] bool grayscale() const { return getBit(ppuMask, 0); }

        // PPU_STATUS - Rendering events ($2002 read)
        [[nodiscard]] bool vBlank() const { return getBit(ppuStatus, 7); }
        void setVBlank() { setBit(ppuStatus, 7); }
        void clearVBlank() { clearBit(ppuStatus, 7); }
        [[nodiscard]] bool sprite0Hit() const { return getBit(ppuStatus, 6); }
        void setSprite0Hit() { setBit(ppuStatus, 6); }
        [[nodiscard]] bool spriteOverflow() const { return getBit(ppuStatus, 5); }
        void setSpriteOverflow() { setBit(ppuStatus, 5); }
    };

    class PPU {
    public:
        explicit PPU(Bus* bus);

        void reset();

        PPURegisters& registers() { return regs_; }
        [[nodiscard]] const PPURegisters& registers() const { return regs_; }

        std::uint8_t registerRead(std::uint16_t addr);
        void registerWrite(std::uint16_t addr, std::uint8_t value);

        [[nodiscard]] std::uint8_t ppuRead(std::uint16_t addr) const;
        void ppuWrite(std::uint16_t addr, std::uint8_t value);

        void step();

        enum class Mirroring {
            HORIZONTAL,
            VERTICAL,
        };
        void setMirroring(const Mirroring mirroring) { mirroring_ = mirroring; }

        [[nodiscard]] std::int32_t scanline() const { return scanline_; }
        [[nodiscard]] std::int32_t cycle() const { return cycle_; }
        [[nodiscard]] std::int64_t frame() const { return frame_; }
        [[nodiscard]] const std::array<std::uint32_t, PPU_FRAME_BUFFER_SIZE>& frameBuffer() const { return frameBuffer_; }

    protected:
        [[nodiscard]] std::uint16_t mirrorNametableAddr(std::uint16_t addr) const;
        static std::uint16_t mirrorPaletteAddr(std::uint16_t addr);

        void updateVBlankNMI();

        void fetchNametableByte();
        void fetchAttributeTableByte();
        void fetchPatternTableTileLo();
        void fetchPatternTableTileHi();

        void updateShiftRegisters();
        void reloadShiftRegisters();

        void incrementScrollX();
        void incrementScrollY();
        void copyScrollX();
        void copyScrollY();

        void fetchBackgroundTileData();
        void renderPixel();

        void evaluateSprites();
        void fetchSprites();
        void shiftSprites();

    private:
        Bus* bus_ = nullptr;
        PPURegisters regs_{};

        std::array<std::uint8_t, PPU_OAM_SIZE> oam_{};
        std::array<std::uint8_t, PPU_NAMETABLE_SIZE> nametable_{};
        std::array<std::uint8_t, PPU_PALETTE_SIZE> palette_{};
        std::array<std::uint32_t, PPU_FRAME_BUFFER_SIZE> frameBuffer_{};

        struct SpriteData {
            std::uint8_t y = 0xFF;
            std::uint8_t tile = 0xFF;
            std::uint8_t attribute = 0xFF;
            std::uint8_t x = 0xFF;
            std::uint8_t patternLo = 0;
            std::uint8_t patternHi = 0;

            [[nodiscard]] int palette() const { return 4 + (attribute & 0x03); }
            [[nodiscard]] bool behindBackground() const { return getBit(attribute, 5); }
            [[nodiscard]] bool horizontalFlip() const { return getBit(attribute, 6); }
            [[nodiscard]] bool verticalFlip() const { return getBit(attribute, 7); }
        };
        std::array<SpriteData, 8> sprites_{};
        int spriteCount_ = 0;
        bool sprite0Rendering_ = false;

        Mirroring mirroring_ = Mirroring::HORIZONTAL;
        std::int32_t scanline_ = 0;
        std::int32_t cycle_ = 0;
        std::int64_t frame_ = 1;
        bool suppressVBlank_ = false;
        bool isVBlankNMIOn_ = false;
        bool isVBlankNMIOnPrev_ = false;
        bool vBlankNMIPending_ = false;

        std::uint8_t nametableByte_ = 0;
        std::uint8_t attrTableByte_ = 0;
        std::uint8_t patternTableTileLo_ = 0;
        std::uint8_t patternTableTileHi_ = 0;
    };

}

#endif //SPIKYNES_PPU_H