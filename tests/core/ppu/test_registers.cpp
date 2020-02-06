#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PPUTestBase : public testing::Test {
protected:
    Emulator emulator;
};

// ==================== PPUCTRL ($2000) ====================

TEST_F(PPUTestBase, PPUCTRL_VBlankNmiEnabled) {
    // Bit 7: Generate NMI at start of VBlank
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().write(0x2000, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().write(0x2000, 0xFF);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().write(0x2000, 0x7F);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());
}

TEST_F(PPUTestBase, PPUCTRL_SpriteSize) {
    // Bit 5: Sprite size (0: 8x8; 1: 8x16)
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 8);

    emulator.ppu().write(0x2000, 0x20);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 16);

    emulator.ppu().write(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 16);

    emulator.ppu().write(0x2000, 0xDF);
    EXPECT_EQ(emulator.ppu().registers().spriteSize(), 8);
}

TEST_F(PPUTestBase, PPUCTRL_SpritePatternTableAddr) {
    // Bit 4: Sprite pattern table address (0: $0000; 1: $1000)
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x0000);

    emulator.ppu().write(0x2000, 0x10);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x1000);

    emulator.ppu().write(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x1000);

    emulator.ppu().write(0x2000, 0xEF);
    EXPECT_EQ(emulator.ppu().registers().spritePatternTableAddr(), 0x0000);
}

TEST_F(PPUTestBase, PPUCTRL_VRamIncrement) {
    // Bit 3: VRAM address increment (0: add 1; 1: add 32)
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 1);

    emulator.ppu().write(0x2000, 0x08);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 32);

    emulator.ppu().write(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 32);

    emulator.ppu().write(0x2000, 0xF7);
    EXPECT_EQ(emulator.ppu().registers().vRamIncrement(), 1);
}

TEST_F(PPUTestBase, PPUCTRL_BaseNametableAddr) {
    // Bits 0-1: Base nametable address
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2000);

    emulator.ppu().write(0x2000, 0x01);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2400);

    emulator.ppu().write(0x2000, 0x02);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2800);

    emulator.ppu().write(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2C00);

    emulator.ppu().write(0x2000, 0xFC);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2000);

    emulator.ppu().write(0x2000, 0xFD);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2400);

    emulator.ppu().write(0x2000, 0xFE);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2800);

    emulator.ppu().write(0x2000, 0xFF);
    EXPECT_EQ(emulator.ppu().registers().baseNametableAddr(), 0x2C00);
}

TEST_F(PPUTestBase, PPUCTRL_MirroredWrite) {
    // PPUCTRL is mirrored every 8 bytes from $2000 to $3FFF
    emulator.ppu().write(0x2008, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().vBlankNmiEnabled());

    emulator.ppu().write(0x3FF8, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().vBlankNmiEnabled());
}

TEST_F(PPUTestBase, PPUCTRL_UpdatesInternalT) {
    // Writing to PPUCTRL copies nametable select (bits 0-1) to t (bits 10-11)
    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0000);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().write(0x2000, 0x01);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0400);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().write(0x2000, 0x02);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0800);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().write(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t & 0x0C00, 0x0C00);
}

TEST_F(PPUTestBase, PPUCTRL_UpdatesT_PreservesOtherBits) {
    // Writing to PPUCTRL should only modify bits 10-11 of t, preserving other bits
    emulator.ppu().registers().t = 0x7FFF;
    emulator.ppu().write(0x2000, 0x00);
    EXPECT_EQ(emulator.ppu().registers().t, 0x73FF);

    emulator.ppu().registers().t = 0x0000;
    emulator.ppu().write(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t, 0x0C00);

    emulator.ppu().registers().t = 0x73FF;
    emulator.ppu().write(0x2000, 0x03);
    EXPECT_EQ(emulator.ppu().registers().t, 0x7FFF);
}

// ==================== PPUMASK ($2001) ====================

TEST_F(PPUTestBase, PPUMASK_EmphasizeBlue) {
    // Bit 7: Emphasize blue
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeBlue());

    emulator.ppu().write(0x2001, 0x80);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeBlue());

    emulator.ppu().write(0x2001, 0x7F);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeBlue());
}

TEST_F(PPUTestBase, PPUMASK_EmphasizeGreen) {
    // Bit 6: Emphasize green
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeGreen());

    emulator.ppu().write(0x2001, 0x40);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeGreen());

    emulator.ppu().write(0x2001, 0xBF);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeGreen());
}

TEST_F(PPUTestBase, PPUMASK_EmphasizeRed) {
    // Bit 5: Emphasize red
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeRed());

    emulator.ppu().write(0x2001, 0x20);
    EXPECT_TRUE(emulator.ppu().registers().emphasizeRed());

    emulator.ppu().write(0x2001, 0xDF);
    EXPECT_FALSE(emulator.ppu().registers().emphasizeRed());
}

TEST_F(PPUTestBase, PPUMASK_SpriteRenderingEnabled) {
    // Bit 4: Show sprites
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().spriteRenderingEnabled());

    emulator.ppu().write(0x2001, 0x10);
    EXPECT_TRUE(emulator.ppu().registers().spriteRenderingEnabled());

    emulator.ppu().write(0x2001, 0xEF);
    EXPECT_FALSE(emulator.ppu().registers().spriteRenderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_BackgroundRenderingEnabled) {
    // Bit 3: Show background
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().backgroundRenderingEnabled());

    emulator.ppu().write(0x2001, 0x08);
    EXPECT_TRUE(emulator.ppu().registers().backgroundRenderingEnabled());

    emulator.ppu().write(0x2001, 0xF7);
    EXPECT_FALSE(emulator.ppu().registers().backgroundRenderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_RenderingEnabled) {
    // Bits 3-4: Rendering enabled if either sprites or background enabled
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().write(0x2001, 0x08);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().write(0x2001, 0x10);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().write(0x2001, 0x18);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().write(0x2001, 0xE7);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());
}

TEST_F(PPUTestBase, PPUMASK_ShowSpritesInLeft) {
    // Bit 2: Show sprites in leftmost 8 pixels
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().showSpritesInLeft());

    emulator.ppu().write(0x2001, 0x04);
    EXPECT_TRUE(emulator.ppu().registers().showSpritesInLeft());

    emulator.ppu().write(0x2001, 0xFB);
    EXPECT_FALSE(emulator.ppu().registers().showSpritesInLeft());
}

TEST_F(PPUTestBase, PPUMASK_ShowBackgroundInLeft) {
    // Bit 1: Show background in leftmost 8 pixels
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().showBackgroundInLeft());

    emulator.ppu().write(0x2001, 0x02);
    EXPECT_TRUE(emulator.ppu().registers().showBackgroundInLeft());

    emulator.ppu().write(0x2001, 0xFD);
    EXPECT_FALSE(emulator.ppu().registers().showBackgroundInLeft());
}

TEST_F(PPUTestBase, PPUMASK_Grayscale) {
    // Bit 0: Grayscale mode
    emulator.ppu().write(0x2001, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().grayscale());

    emulator.ppu().write(0x2001, 0x01);
    EXPECT_TRUE(emulator.ppu().registers().grayscale());

    emulator.ppu().write(0x2001, 0xFE);
    EXPECT_FALSE(emulator.ppu().registers().grayscale());
}

TEST_F(PPUTestBase, PPUMASK_MirroredWrite) {
    // PPUMASK is mirrored every 8 bytes from $2000 to $3FFF
    emulator.ppu().write(0x2009, 0x18);
    EXPECT_TRUE(emulator.ppu().registers().renderingEnabled());

    emulator.ppu().write(0x3FF9, 0x00);
    EXPECT_FALSE(emulator.ppu().registers().renderingEnabled());
}
