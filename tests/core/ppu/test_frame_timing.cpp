#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PPUFrameTimingTest : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initCHR(CHR_ROM_BANK_SIZE);
        emulator.reset();
    }

    void stepPPU(const int cycles) {
        for (int i = 0; i < cycles; ++i) {
            emulator.ppu().step();
        }
    }

    void expectNextPosition(const int frame, const int scanline, const int cycle) {
        EXPECT_EQ(emulator.ppu().frame(), frame);
        EXPECT_EQ(emulator.ppu().scanline(), scanline);
        EXPECT_EQ(emulator.ppu().cycle(), cycle);
    }
};

// ==================== VBlank Flag ====================

TEST_F(PPUFrameTimingTest, VBlank_ClearAfterReset) {
    // VBlank flag should be clear after reset
    expectNextPosition(1, 0, 0);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, VBlank_NotSetBeforeScanline241) {
    // VBlank is not set until we reach scanline 241, cycle 1
    stepPPU(241 * 341 + 1);
    expectNextPosition(1, 241, 1);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, VBlank_SetAtScanline241Cycle0) {
    // VBlank is set at scanline 241, cycle 1
    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, VBlank_ClearedByReadingPPUSTATUS) {
    // Reading PPUSTATUS ($2002) clears VBlank flag
    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());

    emulator.ppu().registerRead(0x2002);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, VBlank_ClearedAtPreRenderScanline) {
    // VBlank is cleared at scanline 261 (pre-render), cycle 1
    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());

    stepPPU(20 * 341);
    expectNextPosition(1, 261, 2);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, VBlank_SetAgainNextFrame) {
    // VBlank is set again at scanline 241 of the next frame
    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());

    emulator.ppu().registerRead(0x2002);
    EXPECT_FALSE(emulator.ppu().registers().vBlank());

    stepPPU(21 * 341 - 1 + 241 * 341 + 1);
    expectNextPosition(2, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
}

// ==================== NMI ====================

TEST_F(PPUFrameTimingTest, NMI_NotTriggeredWhenDisabled) {
    // NMI is not triggered when PPUCTRL bit 7 is 0
    emulator.ppu().registerWrite(0x2000, 0x00);
    EXPECT_FALSE(emulator.cpu().nmiPending());

    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
    EXPECT_FALSE(emulator.cpu().nmiPending());
}

TEST_F(PPUFrameTimingTest, NMI_TriggeredWhenEnabled) {
    // NMI is triggered at VBlank when PPUCTRL bit 7 is 1
    emulator.ppu().registerWrite(0x2000, 0x80);
    EXPECT_FALSE(emulator.cpu().nmiPending());

    stepPPU(241 * 341 + 2);
    expectNextPosition(1, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
    EXPECT_TRUE(emulator.cpu().nmiPending());
}

// ==================== Odd Frame Skip ====================

TEST_F(PPUFrameTimingTest, OddFrameSkip_Disabled_WhenRenderingOff) {
    // No odd frame skip when rendering is disabled (full 89342 cycles per frame)
    emulator.ppu().registerWrite(0x2001, 0x00);

    stepPPU(262 * 341 + 1);
    expectNextPosition(2, 0, 1);

    stepPPU(241 * 341 + 1);
    expectNextPosition(2, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
}

TEST_F(PPUFrameTimingTest, OddFrameSkip_Enabled_WhenRenderingOn) {
    // Odd frame is 1 cycle shorter when rendering is enabled (89341 cycles)
    emulator.ppu().registerWrite(0x2001, 0x18);

    stepPPU(262 * 341);
    expectNextPosition(2, 0, 1);

    stepPPU(241 * 341 + 1);
    expectNextPosition(2, 241, 2);
    EXPECT_TRUE(emulator.ppu().registers().vBlank());
}
