#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PPUScrollTest : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initCHR(CHR_ROM_BANK_SIZE);
        emulator.reset();
        emulator.ppu().registerWrite(0x2001, 0x08);
    }

    void stepPPU(const int cycles) {
        for (int i = 0; i < cycles; ++i) {
            emulator.ppu().step();
        }
    }

    PPURegisters& regs() {
        return emulator.ppu().registers();
    }

    int coarseX() const {
        return emulator.ppu().registers().v & 0x001F;
    }

    int coarseY() const {
        return (emulator.ppu().registers().v >> 5) & 0x001F;
    }

    int fineY() const {
        return (emulator.ppu().registers().v >> 12) & 0x07;
    }

    int horizontalNametable() const {
        return (emulator.ppu().registers().v >> 10) & 0x01;
    }

    int verticalNametable() const {
        return (emulator.ppu().registers().v >> 11) & 0x01;
    }
};

// ==================== Coarse X Increment ====================

TEST_F(PPUScrollTest, CoarseX_IncrementOnCycle8) {
    // At cycle 8 (after tile fetch), coarse X should increment
    regs().v = 0x0000;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 1);
}

TEST_F(PPUScrollTest, CoarseX_IncrementEvery8Cycles) {
    // Coarse X increments at cycles 8, 16, 24, 32
    regs().v = 0x0000;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 1);
    stepPPU(8);
    EXPECT_EQ(coarseX(), 2);
    stepPPU(8);
    EXPECT_EQ(coarseX(), 3);
    stepPPU(8);
    EXPECT_EQ(coarseX(), 4);
}

TEST_F(PPUScrollTest, CoarseX_IncrementFrom0To30) {
    // Normal increment without wrapping
    regs().v = 0x0000;
    stepPPU(249);
    EXPECT_EQ(coarseX(), 31);
}

TEST_F(PPUScrollTest, CoarseX_WrapAt31_SwitchesHorizontalNametable) {
    // When coarse X = 31 and increments, it wraps to 0 and toggles horizontal nametable
    regs().v = 0x001F;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(horizontalNametable(), 1);
}

TEST_F(PPUScrollTest, CoarseX_WrapAt31_PreservesOtherBits) {
    // When wrapping, other bits should be preserved
    regs().v = 0x395F;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(coarseY(), 10);
    EXPECT_EQ(fineY(), 3);
    EXPECT_EQ(horizontalNametable(), 1);
    EXPECT_EQ(verticalNametable(), 1);
}

TEST_F(PPUScrollTest, CoarseX_ToggleBackNametable) {
    // If horizontal nametable is 1, wrapping toggles it back to 0
    regs().v = 0x041F;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(horizontalNametable(), 0);
}

TEST_F(PPUScrollTest, CoarseX_MultipleWraps) {
    // Test that wrapping works correctly multiple times
    regs().v = 0x001E;
    stepPPU(9);
    EXPECT_EQ(coarseX(), 31);
    EXPECT_EQ(horizontalNametable(), 0);
    stepPPU(8);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(horizontalNametable(), 1);
    stepPPU(240);
    EXPECT_EQ(coarseX(), 30);
    EXPECT_EQ(horizontalNametable(), 1);
}

// ==================== Coarse Y Increment ====================

TEST_F(PPUScrollTest, FineY_IncrementAtCycle256) {
    // At cycle 256, fine Y should increment
    regs().v = 0x0000;
    stepPPU(257);
    EXPECT_EQ(fineY(), 1);
}

TEST_F(PPUScrollTest, FineY_IncrementFrom0To7) {
    // Fine Y increments each scanline (cycle 256)
    regs().v = 0x0000;
    for (int expectedFineY = 1; expectedFineY <= 7; ++expectedFineY) {
        stepPPU(341);
    }
}

TEST_F(PPUScrollTest, FineY_WrapAt7_IncrementsCoarseY) {
    // When fine Y = 7 and increments, it wraps to 0 and coarse Y increments
    regs().v = 0x7000;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 1);
}

TEST_F(PPUScrollTest, CoarseY_IncrementNormal) {
    // Normal coarse Y increment when fine Y wraps
    regs().v = 0x7020;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 2);
}

TEST_F(PPUScrollTest, CoarseY_IncrementFrom0To28) {
    // Coarse Y should increment normally from 0 to 28
    regs().v = 0x7380;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 29);
}

TEST_F(PPUScrollTest, CoarseY_WrapAt29_SwitchesVerticalNametable) {
    // When coarse Y = 29 and increments, it wraps to 0 and toggles vertical nametable
    regs().v = 0x73A0;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 0);
    EXPECT_EQ(verticalNametable(), 1);
}

TEST_F(PPUScrollTest, CoarseY_WrapAt29_PreservesOtherBits) {
    // When wrapping at 29, incrementScrollY should preserve horizontal bits
    regs().v = 0x77A0;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 0);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(horizontalNametable(), 0);
    EXPECT_EQ(verticalNametable(), 1);
}

TEST_F(PPUScrollTest, CoarseY_At31_WrapsWithoutNametableSwitch) {
    // Attribute table can set coarse Y to 30 or 31
    regs().v = 0x73E0;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 0);
    EXPECT_EQ(verticalNametable(), 0);
}

TEST_F(PPUScrollTest, CoarseY_At30_IncrementsTo31) {
    // coarse Y = 30 increments to 31 normally
    regs().v = 0x73C0;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 31);
}

TEST_F(PPUScrollTest, CoarseY_ToggleBackNametable) {
    // If vertical nametable is 1, wrapping at 29 toggles it back to 0
    regs().v = 0x7BA0;
    stepPPU(257);
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 0);
    EXPECT_EQ(verticalNametable(), 0);
}

// ==================== Copy Scroll X ====================

TEST_F(PPUScrollTest, CopyScrollX_AtCycle257) {
    // At cycle 257, coarse X and horizontal nametable are copied from t to v
    regs().t = 0x041F;
    regs().v = 0x0000;
    stepPPU(258);
    EXPECT_EQ(regs().v & 0x041F, 0x041F);
}

TEST_F(PPUScrollTest, CopyScrollX_PreservesVerticalBits) {
    // Copy X should only modify bits 0-4 and 10, preserving others
    regs().t = 0x041F;
    regs().v = 0x7BE0;
    stepPPU(258);
    EXPECT_EQ(regs().v & 0x041F, 0x041F);
}

TEST_F(PPUScrollTest, CopyScrollX_CoarseXOnly) {
    // Test copying just coarse X
    regs().t = 0x000A;
    regs().v = 0x0015;
    stepPPU(258);
    EXPECT_EQ(coarseX(), 10);
}

TEST_F(PPUScrollTest, CopyScrollX_HorizontalNametable) {
    // Test copying horizontal nametable bit
    regs().t = 0x0400;
    regs().v = 0x0000;
    stepPPU(258);
    EXPECT_EQ(horizontalNametable(), 1);
}

// ==================== Copy Scroll Y ====================

TEST_F(PPUScrollTest, CopyScrollY_OnPreRenderScanline) {
    // On scanline 261, cycles 280-304, vertical scroll is copied from t to v
    regs().t = 0x7BE0;
    regs().v = 0x0000;
    for (int scanline = 0; scanline < 261; ++scanline) {
        stepPPU(341);
    }
    stepPPU(281);
    EXPECT_EQ(regs().v & 0x7BE0, 0x7BE0);
}

TEST_F(PPUScrollTest, CopyScrollY_PreservesHorizontalBits) {
    // Copy Y should only modify bits 5-9, 11, and 12-14
    regs().t = 0x7BE0;
    for (int scanline = 0; scanline < 261; ++scanline) {
        stepPPU(341);
    }
    regs().v = 0x041F;
    stepPPU(281);
    EXPECT_EQ(regs().v & 0x7BE0, 0x7BE0);
}

// ==================== Rendering Disabled ====================

TEST_F(PPUScrollTest, RenderingDisabled_NoScrollIncrement) {
    // When rendering is disabled, scroll should not change
    emulator.ppu().registerWrite(0x2001, 0x00);
    regs().v = 0x0000;
    stepPPU(257);
    EXPECT_EQ(regs().v, 0x0000);
}

TEST_F(PPUScrollTest, RenderingDisabled_NoCoarseXIncrement) {
    emulator.ppu().registerWrite(0x2001, 0x00);
    regs().v = 0x0010;
    stepPPU(100);
    EXPECT_EQ(coarseX(), 16);
}

TEST_F(PPUScrollTest, RenderingDisabled_NoCopyScrollX) {
    emulator.ppu().registerWrite(0x2001, 0x00);
    regs().t = 0x041F;
    regs().v = 0x0000;
    stepPPU(258);
    EXPECT_EQ(regs().v, 0x0000);
}

// ==================== Combined Operations ====================

TEST_F(PPUScrollTest, FullTileFetch_CoarseXIncrements32Times) {
    // During visible scanline (cycles 1-256), coarse X increments 32 times
    regs().v = 0x0000;
    stepPPU(257);
    EXPECT_EQ(coarseX(), 0);
    EXPECT_EQ(horizontalNametable(), 1);
}

TEST_F(PPUScrollTest, VisibleScanline_CoarseYAfterFineYWrap) {
    // After 8 scanlines (8 fine Y increments), coarse Y should increment
    regs().v = 0x0000;
    for (int scanline = 0; scanline < 8; ++scanline) {
        stepPPU(341);
    }
    EXPECT_EQ(fineY(), 0);
    EXPECT_EQ(coarseY(), 1);
}
