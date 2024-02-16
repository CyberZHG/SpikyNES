#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PPUTestBase : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initChrRom(CHR_ROM_BANK_SIZE);
        emulator.reset();
    }
};

// ==================== PPUCTRL ($2000) ====================

TEST_F(PPUTestBase, PPUCTRL_VBlankNmiEnabled) {
    // Bit 7: Generate NMI at start of VBlank
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().registerWrite(0x2000, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().registerWrite(0x2000, 0x7F);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());
}

TEST_F(PPUTestBase, PPUCTRL_SpriteSize) {
    // Bit 5: Sprite size (0: 8x8; 1: 8x16)
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 8);

    emulator.ppu().registerWrite(0x2000, 0x20);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 16);

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 16);

    emulator.ppu().registerWrite(0x2000, 0xDF);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 8);
}

TEST_F(PPUTestBase, PPUCTRL_BackgroundPatternTableAddr) {
    // Bit 4: Background pattern table address (0: $0000; 1: $1000)
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().backgroundPatternTableAddr(), 0x0000);

    emulator.ppu().registerWrite(0x2000, 0x10);
    EXPECT_EQ(emulator.ppu().registers().backgroundPatternTableAddr(), 0x1000);

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().backgroundPatternTableAddr(), 0x1000);

    emulator.ppu().registerWrite(0x2000, 0xEF);
    EXPECT_EQ(emulator.ppu().registers().backgroundPatternTableAddr(), 0x0000);
}

TEST_F(PPUTestBase, PPUCTRL_SpritePatternTableAddr) {
    // Bit 3: Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000)
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x0000);

    emulator.ppu().registerWrite(0x2000, 0x08);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x1000);

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x1000);

    emulator.ppu().registerWrite(0x2000, 0xF7);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x0000);
}

TEST_F(PPUTestBase, PPUCTRL_VRamIncrement) {
    // Bit 2: VRAM address increment (0: add 1; 1: add 32)
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 1);

    emulator.ppu().registerWrite(0x2000, 0x04);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 32);

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 32);

    emulator.ppu().registerWrite(0x2000, 0xFB);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 1);
}

TEST_F(PPUTestBase, PPUCTRL_BaseNametableAddr) {
    // Bits 0-1: Base nametable address
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2000);

    emulator.ppu().registerWrite(0x2000, 0x01);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2400);

    emulator.ppu().registerWrite(0x2000, 0x02);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2800);

    emulator.ppu().registerWrite(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2C00);

    emulator.ppu().registerWrite(0x2000, 0xFC);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2000);

    emulator.ppu().registerWrite(0x2000, 0xFD);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2400);

    emulator.ppu().registerWrite(0x2000, 0xFE);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2800);

    emulator.ppu().registerWrite(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2C00);
}

TEST_F(PPUTestBase, PPUCTRL_MirroredWrite) {
    // PPUCTRL is mirrored every 8 bytes from $2000 to $3FFF
    emulator.ppu().registerWrite(0x2008, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().registerWrite(0x3FF8, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());
}

TEST_F(PPUTestBase, PPUCTRL_UpdatesInternalT) {
    // Writing to PPUCTRL copies nametable select (bits 0-1) to t (bits 10-11)
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0000);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registerWrite(0x2000, 0x01);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0400);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registerWrite(0x2000, 0x02);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0800);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registerWrite(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0C00);
}

TEST_F(PPUTestBase, PPUCTRL_UpdatesT_PreservesOtherBits) {
    // Writing to PPUCTRL should only modify bits 10-11 of t, preserving other bits
    emulator.ppu().registers().t = 0x7FFF;
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().t, 0x73FF);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registerWrite(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t, 0x0C00);

    emulator.ppu().registers().t = 0x73FF;
    emulator.ppu().registerWrite(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t, 0x7FFF);
}

// ==================== PPUMASK ($2001) ====================

TEST_F(PPUTestBase, PPUMASK_EmphasizeBlue) {
    // Bit 7: Emphasize blue
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeBlue());

    emulator.ppu().registerWrite(0x2001, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeBlue());

    emulator.ppu().registerWrite(0x2001, 0x7F);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeBlue());
}

TEST_F(PPUTestBase, PPUMASK_EmphasizeGreen) {
    // Bit 6: Emphasize green
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeGreen());

    emulator.ppu().registerWrite(0x2001, 0x40);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeGreen());

    emulator.ppu().registerWrite(0x2001, 0xBF);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeGreen());
}

TEST_F(PPUTestBase, PPUMASK_EmphasizeRed) {
    // Bit 5: Emphasize red
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeRed());

    emulator.ppu().registerWrite(0x2001, 0x20);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeRed());

    emulator.ppu().registerWrite(0x2001, 0xDF);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeRed());
}

TEST_F(PPUTestBase, PPUMASK_SpriteRenderingEnabled) {
    // Bit 4: Show sprites
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().spriteRenderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0x10);
    EXPECT_TRUE(emulator.ppu().registers().spriteRenderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0xEF);
    EXPECT_FALSE(emulator.ppu().registers().spriteRenderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_BackgroundRenderingEnabled) {
    // Bit 3: Show background
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().backgroundRenderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0x08);
    EXPECT_TRUE(emulator.ppu().registers().backgroundRenderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0xF7);
    EXPECT_FALSE(emulator.ppu().registers().backgroundRenderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_RenderingEnabled) {
    // Bits 3-4: Rendering enabled if either sprites or background enabled
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0x08);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0x10);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0x18);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().registerWrite(0x2001, 0xE7);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_ShowSpritesInLeft) {
    // Bit 2: Show sprites in leftmost 8 pixels
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().showSpritesInLeft());

    emulator.ppu().registerWrite(0x2001, 0x04);
    EXPECT_TRUE(emulator.ppu().registers().showSpritesInLeft());

    emulator.ppu().registerWrite(0x2001, 0xFB);
    EXPECT_FALSE(emulator.ppu().registers().showSpritesInLeft());
}

TEST_F(PPUTestBase, PPUMASK_ShowBackgroundInLeft) {
    // Bit 1: Show background in leftmost 8 pixels
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().showBackgroundInLeft());

    emulator.ppu().registerWrite(0x2001, 0x02);
    EXPECT_TRUE(emulator.ppu().registers().showBackgroundInLeft());

    emulator.ppu().registerWrite(0x2001, 0xFD);
    EXPECT_FALSE(emulator.ppu().registers().showBackgroundInLeft());
}

TEST_F(PPUTestBase, PPUMASK_Grayscale) {
    // Bit 0: Grayscale mode
    emulator.ppu().registerWrite(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().grayscale());

    emulator.ppu().registerWrite(0x2001, 0x01);
    EXPECT_TRUE(emulator.ppu().registers().grayscale());

    emulator.ppu().registerWrite(0x2001, 0xFE);
    EXPECT_FALSE(emulator.ppu().registers().grayscale());
}

TEST_F(PPUTestBase, PPUMASK_MirroredWrite) {
    // PPUMASK is mirrored every 8 bytes from $2000 to $3FFF
    emulator.ppu().registerWrite(0x2009, 0x18);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().registerWrite(0x3FF9, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());
}

// ==================== PPUSTATUS ($2002) ====================

TEST_F(PPUTestBase, PPUSTATUS_VBlank) {
    // Bit 7: VBlank flag
    emulator.ppu().registers().ppuStatus = 0x00;
    EXPECT_FALSE(emulator.ppu().registers().vBlank());

    emulator.ppu().registers().ppuStatus = 0x80;
    EXPECT_TRUE(emulator.ppu().registers().vBlank());

    emulator.ppu().registers().ppuStatus = 0x7F;
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUTestBase, PPUSTATUS_ClearVBlank) {
    // clearVBlank() should clear bit 7
    emulator.ppu().registers().ppuStatus = 0xFF;
    emulator.ppu().registers().clearVBlank();
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
    EXPECT_EQ(emulator.ppu().registers().ppuStatus, 0x7F);
}

TEST_F(PPUTestBase, PPUSTATUS_Sprite0Hit) {
    // Bit 6: Sprite 0 hit flag
    emulator.ppu().registers().ppuStatus = 0x00;
    EXPECT_FALSE(emulator.ppu().registers().sprite0Hit());

    emulator.ppu().registers().ppuStatus = 0x40;
    EXPECT_TRUE(emulator.ppu().registers().sprite0Hit());

    emulator.ppu().registers().ppuStatus = 0xBF;
    EXPECT_FALSE(emulator.ppu().registers().sprite0Hit());
}

TEST_F(PPUTestBase, PPUSTATUS_SpriteOverflow) {
    // Bit 5: Sprite overflow flag
    emulator.ppu().registers().ppuStatus = 0x00;
    EXPECT_FALSE(emulator.ppu().registers().spriteOverflow());

    emulator.ppu().registers().ppuStatus = 0x20;
    EXPECT_TRUE(emulator.ppu().registers().spriteOverflow());

    emulator.ppu().registers().ppuStatus = 0xDF;
    EXPECT_FALSE(emulator.ppu().registers().spriteOverflow());
}

TEST_F(PPUTestBase, PPUSTATUS_ReadClearsVBlank) {
    // Reading PPUSTATUS clears VBlank flag (bit 7)
    emulator.ppu().registers().ppuStatus = 0xFF;
    emulator.ppu().registerRead(0x2002);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
    EXPECT_EQ(emulator.ppu().registers().ppuStatus & 0x80, 0x00);
}

TEST_F(PPUTestBase, PPUSTATUS_ReadResetsAddressLatch) {
    // Reading PPUSTATUS resets the address latch (w = 0)
    emulator.ppu().registers().w = 1;
    emulator.ppu().registerRead(0x2002);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

TEST_F(PPUTestBase, PPUSTATUS_ReadReturnsUpperBits) {
    // Reading PPUSTATUS returns bits 7-5, lower bits are from internal latch
    emulator.ppu().registers().ppuStatus = 0xE0;
    auto result = emulator.ppu().registerRead(0x2002);
    EXPECT_EQ(result & 0xE0, 0xE0);
}

TEST_F(PPUTestBase, PPUSTATUS_MirroredRead) {
    // PPUSTATUS is mirrored every 8 bytes from $2000 to $3FFF
    emulator.ppu().registers().ppuStatus = 0x80;
    emulator.ppu().registers().w = 1;
    emulator.ppu().registerRead(0x200A);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

// ==================== PPUSCROLL ($2005) ====================

TEST_F(PPUTestBase, PPUSCROLL_FirstWrite_SetsX) {
    // First write (w=0): sets fine X (x) and coarse X (t bits 0-4)
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().x = 0;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0x7D);
    EXPECT_EQ(emulator.ppu().registers().x, 0x05);
    EXPECT_EQ(emulator.ppu().registers().t & 0x001F, 0x0F);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
}

TEST_F(PPUTestBase, PPUSCROLL_FirstWrite_PreservesT) {
    // First write should only modify bits 0-4 of t, preserving other bits
    emulator.ppu().registers().t = 0x7FE0;
    emulator.ppu().registers().x = 0;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().x, 0x07);
    EXPECT_EQ(emulator.ppu().registers().t, 0x7FFF);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
}

TEST_F(PPUTestBase, PPUSCROLL_SecondWrite_SetsY) {
    // Second write (w=1): sets fine Y (t bits 12-14) and coarse Y (t bits 5-9)
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().w = 1;

    emulator.ppu().registerWrite(0x2005, 0x7D);
    EXPECT_EQ(emulator.ppu().registers().t & 0x7000, 0x5000);
    EXPECT_EQ(emulator.ppu().registers().t & 0x03E0, 0x01E0);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

TEST_F(PPUTestBase, PPUSCROLL_SecondWrite_PreservesT) {
    // Second write should only modify bits 5-9 and 12-14 of t
    emulator.ppu().registers().t = 0x0C1F;
    emulator.ppu().registers().w = 1;

    emulator.ppu().registerWrite(0x2005, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().t, 0x7FFF);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

TEST_F(PPUTestBase, PPUSCROLL_DoubleWrite_FullScroll) {
    // Two writes set complete scroll position
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().x = 0;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerWrite(0x2005, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
    EXPECT_EQ(emulator.ppu().registers().t, 0x0000);
    EXPECT_EQ(emulator.ppu().registers().x, 0x00);
}

TEST_F(PPUTestBase, PPUSCROLL_DoubleWrite_MaxScroll) {
    // Two writes with max values
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().x = 0;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0xFF);
    emulator.ppu().registerWrite(0x2005, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().x, 0x07);
    EXPECT_EQ(emulator.ppu().registers().t & 0x001F, 0x1F);
    EXPECT_EQ(emulator.ppu().registers().t & 0x03E0, 0x03E0);
    EXPECT_EQ(emulator.ppu().registers().t & 0x7000, 0x7000);
}

TEST_F(PPUTestBase, PPUSCROLL_WToggle) {
    // w toggles between 0 and 1 on each write
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerWrite(0x2005, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 0);

    emulator.ppu().registerWrite(0x2005, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
}

TEST_F(PPUTestBase, PPUSCROLL_StatusReadResetsW) {
    // Reading PPUSTATUS resets w, affecting PPUSCROLL sequence
    emulator.ppu().registers().w = 0;
    emulator.ppu().registerWrite(0x2005, 0x10);
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerRead(0x2002);
    EXPECT_EQ(emulator.ppu().registers().w, 0);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().x = 0;
    emulator.ppu().registerWrite(0x2005, 0x08);
    EXPECT_EQ(emulator.ppu().registers().x, 0x00);
    EXPECT_EQ(emulator.ppu().registers().t & 0x001F, 0x01);
}

// ==================== OAMADDR ($2003) ====================

TEST_F(PPUTestBase, OAMADDR_Write) {
    // Writing to OAMADDR sets the OAM address
    emulator.ppu().registerWrite(0x2003, 0x00);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x00);

    emulator.ppu().registerWrite(0x2003, 0x40);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x40);

    emulator.ppu().registerWrite(0x2003, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0xFF);
}

TEST_F(PPUTestBase, OAMADDR_MirroredWrite) {
    // OAMADDR is mirrored every 8 bytes
    emulator.ppu().registerWrite(0x200B, 0x55);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x55);

    emulator.ppu().registerWrite(0x3FFB, 0xAA);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0xAA);
}

// ==================== OAMDATA ($2004) ====================

TEST_F(PPUTestBase, OAMDATA_WriteAndRead) {
    // Writing to OAMDATA stores data in OAM at oamAddr
    emulator.ppu().registerWrite(0x2003, 0x00);
    emulator.ppu().registerWrite(0x2004, 0x42);

    emulator.ppu().registerWrite(0x2003, 0x00);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x42);
}

TEST_F(PPUTestBase, OAMDATA_WriteIncrementsAddr) {
    // Writing to OAMDATA increments oamAddr by 1
    emulator.ppu().registerWrite(0x2003, 0x00);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x00);

    emulator.ppu().registerWrite(0x2004, 0x11);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x01);

    emulator.ppu().registerWrite(0x2004, 0x22);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x02);

    emulator.ppu().registerWrite(0x2004, 0x33);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x03);
}

TEST_F(PPUTestBase, OAMDATA_WriteWrapsAt256) {
    // oamAddr wraps around at 256
    emulator.ppu().registerWrite(0x2003, 0xFF);
    emulator.ppu().registerWrite(0x2004, 0x99);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x00);
}

TEST_F(PPUTestBase, OAMDATA_ReadDoesNotIncrementAddr) {
    // Reading OAMDATA does not increment oamAddr
    emulator.ppu().registerWrite(0x2003, 0x10);
    emulator.ppu().registerWrite(0x2004, 0xAB);

    emulator.ppu().registerWrite(0x2003, 0x10);
    emulator.ppu().registerRead(0x2004);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x10);

    emulator.ppu().registerRead(0x2004);
    EXPECT_EQ(emulator.ppu().registers().oamAddr, 0x10);
}

TEST_F(PPUTestBase, OAMDATA_WriteMultipleSprites) {
    // Write sprite data
    emulator.ppu().registerWrite(0x2003, 0x00);

    emulator.ppu().registerWrite(0x2004, 0x10);
    emulator.ppu().registerWrite(0x2004, 0x01);
    emulator.ppu().registerWrite(0x2004, 0x00);
    emulator.ppu().registerWrite(0x2004, 0x20);

    emulator.ppu().registerWrite(0x2004, 0x30);
    emulator.ppu().registerWrite(0x2004, 0x02);
    emulator.ppu().registerWrite(0x2004, 0x01);
    emulator.ppu().registerWrite(0x2004, 0x40);

    emulator.ppu().registerWrite(0x2003, 0x00);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x10);
    emulator.ppu().registerWrite(0x2003, 0x01);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x01);
    emulator.ppu().registerWrite(0x2003, 0x02);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x00);
    emulator.ppu().registerWrite(0x2003, 0x03);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x20);

    emulator.ppu().registerWrite(0x2003, 0x04);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x30);
    emulator.ppu().registerWrite(0x2003, 0x05);
    EXPECT_EQ(emulator.ppu().registerRead(0x2004), 0x02);
}

TEST_F(PPUTestBase, OAMDATA_MirroredAccess) {
    // OAMDATA is mirrored every 8 bytes
    emulator.ppu().registerWrite(0x2003, 0x00);
    emulator.ppu().registerWrite(0x200C, 0x77);

    emulator.ppu().registerWrite(0x2003, 0x00);
    EXPECT_EQ(emulator.ppu().registerRead(0x3FFC), 0x77);
}

// ==================== PPUADDR ($2006) ====================

TEST_F(PPUTestBase, PPUADDR_FirstWrite_SetsHighByte) {
    // First write (w=0): sets high byte of t (bits 8-13), clears bit 14
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2006, 0x21);
    EXPECT_EQ(emulator.ppu().registers().t & 0x3F00, 0x2100);
    EXPECT_EQ(emulator.ppu().registers().t & 0x4000, 0x0000);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
}

TEST_F(PPUTestBase, PPUADDR_FirstWrite_ClearsBit14) {
    // First write clears bit 14 of t
    emulator.ppu().registers().t = 0x7FFF;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2006, 0x3F);
    EXPECT_EQ(emulator.ppu().registers().t & 0x4000, 0x0000);
}

TEST_F(PPUTestBase, PPUADDR_FirstWrite_PreservesLowByte) {
    // First write should preserve bits 0-7 of t
    emulator.ppu().registers().t = 0x00FF;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2006, 0x20);
    EXPECT_EQ(emulator.ppu().registers().t & 0x00FF, 0x00FF);
    EXPECT_EQ(emulator.ppu().registers().t & 0x3F00, 0x2000);
}

TEST_F(PPUTestBase, PPUADDR_SecondWrite_SetsLowByte) {
    // Second write (w=1): sets low byte of t (bits 0-7)
    emulator.ppu().registers().t = 0x2100;
    emulator.ppu().registers().w = 1;

    emulator.ppu().registerWrite(0x2006, 0x05);
    EXPECT_EQ(emulator.ppu().registers().t & 0x00FF, 0x0005);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

TEST_F(PPUTestBase, PPUADDR_SecondWrite_CopiesTtoV) {
    // Second write copies t to v
    emulator.ppu().registers().t = 0x2100;
    emulator.ppu().registers().v = 0x0000;
    emulator.ppu().registers().w = 1;

    emulator.ppu().registerWrite(0x2006, 0x05);
    EXPECT_EQ(emulator.ppu().registers().v, 0x2105);
}

TEST_F(PPUTestBase, PPUADDR_DoubleWrite_SetsFullAddress) {
    // Two writes set the full VRAM address
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().registers().v = 0x0000;
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2006, 0x21);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0000);

    emulator.ppu().registerWrite(0x2006, 0x08);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
    EXPECT_EQ(emulator.ppu().registers().v, 0x2108);
    EXPECT_EQ(emulator.ppu().registers().t, 0x2108);
}

TEST_F(PPUTestBase, PPUADDR_WToggle) {
    // w toggles between 0 and 1 on each write
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2006, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerWrite(0x2006, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 0);

    emulator.ppu().registerWrite(0x2006, 0x00);
    EXPECT_EQ(emulator.ppu().registers().w, 1);
}

TEST_F(PPUTestBase, PPUADDR_StatusReadResetsW) {
    // Reading PPUSTATUS resets w
    emulator.ppu().registers().w = 0;
    emulator.ppu().registerWrite(0x2006, 0x21);
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerRead(0x2002);
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

TEST_F(PPUTestBase, PPUADDR_SharesWWithScroll) {
    // PPUADDR and PPUSCROLL share the w toggle
    emulator.ppu().registers().w = 0;

    emulator.ppu().registerWrite(0x2005, 0x00);  // PPUSCROLL first write
    EXPECT_EQ(emulator.ppu().registers().w, 1);

    emulator.ppu().registerWrite(0x2006, 0x20);  // PPUADDR second write
    EXPECT_EQ(emulator.ppu().registers().w, 0);
}

// ==================== PPUDATA ($2007) ====================

TEST_F(PPUTestBase, PPUDATA_WriteIncrementsV_By1) {
    // Writing PPUDATA increments v by 1 when PPUCTRL bit 2 is 0
    emulator.ppu().registerWrite(0x2000, 0x00);
    emulator.ppu().registers().v = 0x0000;

    emulator.ppu().registerWrite(0x2007, 0x42);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0001);

    emulator.ppu().registerWrite(0x2007, 0x43);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0002);
}

TEST_F(PPUTestBase, PPUDATA_WriteIncrementsV_By32) {
    // Writing PPUDATA increments v by 32 when PPUCTRL bit 2 is 1
    emulator.ppu().registerWrite(0x2000, 0x04);
    emulator.ppu().registers().v = 0x0000;
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 32);

    emulator.ppu().registerWrite(0x2007, 0x42);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0020);

    emulator.ppu().registerWrite(0x2007, 0x43);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0040);
}

TEST_F(PPUTestBase, PPUDATA_ReadIncrementsV_By1) {
    // Reading PPUDATA increments v by 1 when PPUCTRL bit 2 is 0
    emulator.ppu().registerWrite(0x2000, 0x00);
    emulator.ppu().registers().v = 0x0000;

    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0001);

    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0002);
}

TEST_F(PPUTestBase, PPUDATA_ReadIncrementsV_By32) {
    // Reading PPUDATA increments v by 32 when PPUCTRL bit 2 is 1
    emulator.ppu().registerWrite(0x2000, 0x04);
    emulator.ppu().registers().v = 0x0000;

    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0020);

    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registers().v, 0x0040);
}

TEST_F(PPUTestBase, PPUDATA_ReadHasBuffer) {
    // Reading PPUDATA returns buffered value
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x0000;
    emulator.ppu().registerWrite(0x2007, 0xAA);
    emulator.ppu().registers().v = 0x0001;
    emulator.ppu().registerWrite(0x2007, 0xBB);
    emulator.ppu().registers().v = 0x0002;
    emulator.ppu().registerWrite(0x2007, 0xCC);

    emulator.ppu().registers().v = 0x0000;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0xAA);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0xBB);
}

TEST_F(PPUTestBase, PPUDATA_PaletteReadNoBuffer) {
    // Reading palette ($3F00-$3FFF) returns data immediately
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x3F00;
    emulator.ppu().registerWrite(0x2007, 0x0F);
    emulator.ppu().registers().v = 0x3F01;
    emulator.ppu().registerWrite(0x2007, 0x10);

    emulator.ppu().registers().v = 0x3F00;
    EXPECT_EQ(emulator.ppu().registerRead(0x2007) & 0x3F, 0x0F);
}

TEST_F(PPUTestBase, PPUDATA_AddressWraps) {
    // v wraps at 0x4000 (14-bit address space)
    emulator.ppu().registerWrite(0x2000, 0x00);
    emulator.ppu().registers().v = 0x3FFF;

    emulator.ppu().registerWrite(0x2007, 0x00);
    EXPECT_EQ(emulator.ppu().registers().v & 0x3FFF, 0x0000);
}

TEST_F(PPUTestBase, PPUDATA_NametableWriteRead) {
    // Write and read nametable VRAM ($2000-$2FFF)
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x2000;
    emulator.ppu().registerWrite(0x2007, 0x41);
    emulator.ppu().registers().v = 0x2001;
    emulator.ppu().registerWrite(0x2007, 0x42);
    emulator.ppu().registers().v = 0x23C0;
    emulator.ppu().registerWrite(0x2007, 0x55);

    emulator.ppu().registers().v = 0x2000;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x41);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x42);

    emulator.ppu().registers().v = 0x23C0;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x55);
}

TEST_F(PPUTestBase, PPUDATA_NametableMirror_3000) {
    // $3000-$3EFF mirrors $2000-$2EFF
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x2000;
    emulator.ppu().registerWrite(0x2007, 0xAB);

    emulator.ppu().registers().v = 0x3000;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0xAB);

    emulator.ppu().registers().v = 0x3100;
    emulator.ppu().registerWrite(0x2007, 0xCD);

    emulator.ppu().registers().v = 0x2100;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0xCD);
}

TEST_F(PPUTestBase, PPUDATA_NametableHorizontalMirroring) {
    // Horizontal mirroring: $2000=$2400, $2800=$2C00
    emulator.ppu().setMirroring(PPU::Mirroring::HORIZONTAL);
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x2000;
    emulator.ppu().registerWrite(0x2007, 0x11);

    emulator.ppu().registers().v = 0x2400;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x11);

    emulator.ppu().registers().v = 0x2800;
    emulator.ppu().registerWrite(0x2007, 0x22);

    emulator.ppu().registers().v = 0x2C00;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x22);
}

TEST_F(PPUTestBase, PPUDATA_NametableVerticalMirroring) {
    // Vertical mirroring: $2000=$2800, $2400=$2C00
    emulator.ppu().setMirroring(PPU::Mirroring::VERTICAL);
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x2000;
    emulator.ppu().registerWrite(0x2007, 0x33);

    emulator.ppu().registers().v = 0x2800;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x33);

    emulator.ppu().registers().v = 0x2400;
    emulator.ppu().registerWrite(0x2007, 0x44);

    emulator.ppu().registers().v = 0x2C00;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x44);
}

TEST_F(PPUTestBase, PPUDATA_AttributeTableWriteRead) {
    // Attribute table at $23C0-$23FF, $27C0-$27FF, etc.
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x23C0;
    emulator.ppu().registerWrite(0x2007, 0xAA);
    emulator.ppu().registers().v = 0x23FF;
    emulator.ppu().registerWrite(0x2007, 0x55);

    emulator.ppu().registers().v = 0x23C0;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0xAA);

    emulator.ppu().registers().v = 0x23FF;
    emulator.ppu().registerRead(0x2007);
    EXPECT_EQ(emulator.ppu().registerRead(0x2007), 0x55);
}

TEST_F(PPUTestBase, PPUDATA_PaletteMirroring) {
    // $3F10, $3F14, $3F18, $3F1C mirror to $3F00, $3F04, $3F08, $3F0C
    emulator.ppu().registerWrite(0x2000, 0x00);

    emulator.ppu().registers().v = 0x3F00;
    emulator.ppu().registerWrite(0x2007, 0x11);
    emulator.ppu().registers().v = 0x3F10;
    EXPECT_EQ(emulator.ppu().registerRead(0x2007) & 0x3F, 0x11);

    emulator.ppu().registers().v = 0x3F14;
    emulator.ppu().registerWrite(0x2007, 0x22);
    emulator.ppu().registers().v = 0x3F04;
    EXPECT_EQ(emulator.ppu().registerRead(0x2007) & 0x3F, 0x22);

    emulator.ppu().registers().v = 0x3F08;
    emulator.ppu().registerWrite(0x2007, 0x33);
    emulator.ppu().registers().v = 0x3F18;
    EXPECT_EQ(emulator.ppu().registerRead(0x2007) & 0x3F, 0x33);

    emulator.ppu().registers().v = 0x3F1C;
    emulator.ppu().registerWrite(0x2007, 0x2A);
    emulator.ppu().registers().v = 0x3F0C;
    EXPECT_EQ(emulator.ppu().registerRead(0x2007) & 0x3F, 0x2A);
}

// ==================== OAMDMA ($4014) ====================

TEST_F(PPUTestBase, OAMDMA_TransferFromPage00) {
    // Writing to $4014 copies 256 bytes from $XX00-$XXFF to OAM
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0000 + i, static_cast<std::uint8_t>(i));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0xA9);  // LDA #$00
    emulator.bus().cpuWrite(programAddr + 1, 0x00);
    emulator.bus().cpuWrite(programAddr + 2, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 3, 0x14);
    emulator.bus().cpuWrite(programAddr + 4, 0x40);
    emulator.cpu().registers().PC = programAddr;
    emulator.execOp();
    emulator.execOp();
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    for (int i = 0; i < 256; ++i) {
        emulator.ppu().registerWrite(0x2003, static_cast<std::uint8_t>(i));
        EXPECT_EQ(emulator.ppu().registerRead(0x2004), static_cast<std::uint8_t>(i));
    }
}

TEST_F(PPUTestBase, OAMDMA_TransferFromPage02) {
    // Test OAM DMA from page $02 ($0200-$02FF, common sprite page)
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0200 + i, static_cast<std::uint8_t>(255 - i));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0xA9);  // LDA #$02
    emulator.bus().cpuWrite(programAddr + 1, 0x02);
    emulator.bus().cpuWrite(programAddr + 2, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 3, 0x14);
    emulator.bus().cpuWrite(programAddr + 4, 0x40);
    emulator.cpu().registers().PC = programAddr;
    emulator.execOp();
    emulator.execOp();
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    for (int i = 0; i < 256; ++i) {
        emulator.ppu().registerWrite(0x2003, static_cast<std::uint8_t>(i));
        EXPECT_EQ(emulator.ppu().registerRead(0x2004), static_cast<std::uint8_t>(255 - i));
    }
}

TEST_F(PPUTestBase, OAMDMA_TransferFromPage07) {
    // Test OAM DMA from page $07 ($0700-$07FF)
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0700 + i, static_cast<std::uint8_t>((i * 3) & 0xFF));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0xA9);  // LDA #$07
    emulator.bus().cpuWrite(programAddr + 1, 0x07);
    emulator.bus().cpuWrite(programAddr + 2, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 3, 0x14);
    emulator.bus().cpuWrite(programAddr + 4, 0x40);
    emulator.cpu().registers().PC = programAddr;
    emulator.execOp();
    emulator.execOp();
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    for (int i = 0; i < 256; ++i) {
        emulator.ppu().registerWrite(0x2003, static_cast<std::uint8_t>(i));
        EXPECT_EQ(emulator.ppu().registerRead(0x2004), static_cast<std::uint8_t>((i * 3) & 0xFF));
    }
}

TEST_F(PPUTestBase, OAMDMA_StartsFromOAMADDR) {
    // OAM DMA should start writing at current OAMADDR and wrap around
    emulator.ppu().registerWrite(0x2003, 0x04);
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0200 + i, static_cast<std::uint8_t>(i + 0x10));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0xA9);  // LDA #$02
    emulator.bus().cpuWrite(programAddr + 1, 0x02);
    emulator.bus().cpuWrite(programAddr + 2, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 3, 0x14);
    emulator.bus().cpuWrite(programAddr + 4, 0x40);
    emulator.cpu().registers().PC = programAddr;
    emulator.execOp();
    emulator.execOp();
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    for (int i = 0; i < 256; ++i) {
        emulator.ppu().registerWrite(0x2003, static_cast<std::uint8_t>((i + 4) & 0xFF));
        EXPECT_EQ(emulator.ppu().registerRead(0x2004), static_cast<std::uint8_t>(i + 0x10));
    }
}

TEST_F(PPUTestBase, OAMDMA_Cycles_OddAlignment) {
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0200 + i, static_cast<std::uint8_t>(i));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0x04);  // NOP $00
    emulator.bus().cpuWrite(programAddr + 1, 0x00);
    emulator.bus().cpuWrite(programAddr + 2, 0xA9);  // LDA #$02
    emulator.bus().cpuWrite(programAddr + 3, 0x02);
    emulator.bus().cpuWrite(programAddr + 4, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 5, 0x14);
    emulator.bus().cpuWrite(programAddr + 6, 0x40);
    emulator.cpu().registers().PC = programAddr;
    for (int i = 0; i < 3; ++i) {
        emulator.execOp();
    }
    const auto cyclesBefore = emulator.cpu().cycles();
    ASSERT_TRUE(cyclesBefore % 2 == 1);
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    const auto cyclesTaken = emulator.cpu().cycles() - cyclesBefore;
    EXPECT_EQ(cyclesTaken, 514);
}

TEST_F(PPUTestBase, OAMDMA_Cycles_EvenAlignment) {
    for (int i = 0; i < 256; ++i) {
        emulator.bus().cpuWrite(0x0200 + i, static_cast<std::uint8_t>(i));
    }
    constexpr std::uint16_t programAddr = 0x0300;
    emulator.bus().cpuWrite(programAddr + 0, 0xEA);  // NOP
    emulator.bus().cpuWrite(programAddr + 1, 0xA9);  // LDA #$02
    emulator.bus().cpuWrite(programAddr + 2, 0x02);
    emulator.bus().cpuWrite(programAddr + 3, 0x8D);  // STA $4014
    emulator.bus().cpuWrite(programAddr + 4, 0x14);
    emulator.bus().cpuWrite(programAddr + 5, 0x40);
    emulator.cpu().registers().PC = programAddr;
    for (int i = 0; i < 3; ++i) {
        emulator.execOp();
    }
    const auto cyclesBefore = emulator.cpu().cycles();
    ASSERT_TRUE(cyclesBefore % 2 == 0);
    while (emulator.cpu().dmaPending()) {
        emulator.step();
    }
    const auto cyclesTaken = emulator.cpu().cycles() - cyclesBefore;
    EXPECT_EQ(cyclesTaken, 513);
}
