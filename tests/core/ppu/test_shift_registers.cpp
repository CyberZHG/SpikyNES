#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PPUShiftRegisterTest : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initChrRom(CHR_ROM_BANK_SIZE);
        emulator.reset();
        emulator.ppu().registerWrite(0x2001, 0x08);
    }

    void stepPPU(const int cycles) {
        for (int i = 0; i < cycles; ++i) {
            emulator.ppu().step();
        }
    }

    void setupNametable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().registers().v = addr;
        emulator.ppu().ppuWrite(addr, value);
    }

    void setupPatternTable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().ppuWrite(addr, value);
    }

    PPURegisters& regs() {
        return emulator.ppu().registers();
    }
};

// ==================== Cycle 0: Idle ====================

TEST_F(PPUShiftRegisterTest, Cycle0_Idle) {
    // Cycle 0 is idle, shift registers should not change
    regs().bgPatternShiftLo = 0xABCD;
    regs().bgPatternShiftHi = 0x1234;
    regs().bgAttrShiftLo = 0x5678;
    regs().bgAttrShiftHi = 0x9ABC;
    stepPPU(1);
    EXPECT_EQ(regs().bgPatternShiftLo, 0xABCD);
    EXPECT_EQ(regs().bgPatternShiftHi, 0x1234);
    EXPECT_EQ(regs().bgAttrShiftLo, 0x5678);
    EXPECT_EQ(regs().bgAttrShiftHi, 0x9ABC);
}

// ==================== Cycles 1-8: First tile fetch ====================

TEST_F(PPUShiftRegisterTest, Cycle1_ShiftLeft) {
    // At cycle 1, shift registers should shift left
    regs().bgPatternShiftLo = 0x0080;
    regs().bgPatternShiftHi = 0x0080;
    regs().bgAttrShiftLo = 0x0080;
    regs().bgAttrShiftHi = 0x0080;
    stepPPU(2);
    EXPECT_EQ(regs().bgPatternShiftLo, 0x0100);
    EXPECT_EQ(regs().bgPatternShiftHi, 0x0100);
    EXPECT_EQ(regs().bgAttrShiftLo, 0x0100);
    EXPECT_EQ(regs().bgAttrShiftHi, 0x0100);
}

TEST_F(PPUShiftRegisterTest, Cycle2_ShiftLeft) {
    // At cycle 2, shift registers continue to shift
    regs().bgPatternShiftLo = 0x0040;
    regs().bgPatternShiftHi = 0x0040;
    regs().bgAttrShiftLo = 0x0040;
    regs().bgAttrShiftHi = 0x0040;
    stepPPU(3);
    EXPECT_EQ(regs().bgPatternShiftLo, 0x0100);
    EXPECT_EQ(regs().bgPatternShiftHi, 0x0100);
    EXPECT_EQ(regs().bgAttrShiftLo, 0x0100);
    EXPECT_EQ(regs().bgAttrShiftHi, 0x0100);
}

TEST_F(PPUShiftRegisterTest, Cycles1To8_ShiftLeft8Times) {
    // After 8 cycles (1-8), shift registers should have shifted 8 times
    regs().bgPatternShiftLo = 0x00FF;
    regs().bgPatternShiftHi = 0x00AA;
    regs().bgAttrShiftLo = 0x0055;
    regs().bgAttrShiftHi = 0x0033;
    stepPPU(9);
    EXPECT_EQ(regs().bgPatternShiftLo & 0xFF00, 0xFF00);
    EXPECT_EQ(regs().bgPatternShiftHi & 0xFF00, 0xAA00);
    EXPECT_EQ(regs().bgAttrShiftLo & 0xFF00, 0x5500);
    EXPECT_EQ(regs().bgAttrShiftHi & 0xFF00, 0x3300);
}

// ==================== Cycle 8: Load new tile data ====================

TEST_F(PPUShiftRegisterTest, Cycle8_LoadNewTileData) {
    // At cycle 8, new tile data should be loaded into low byte of shift registers
    regs().v = 0x0000;
    emulator.ppu().ppuWrite(0x2000, 0x01);
    setupPatternTable(0x0010, 0xAA);
    setupPatternTable(0x0018, 0x55);
    regs().bgPatternShiftLo = 0x0000;
    regs().bgPatternShiftHi = 0x0000;
    stepPPU(9);
    EXPECT_EQ(regs().bgPatternShiftLo & 0x00FF, 0xAA);
    EXPECT_EQ(regs().bgPatternShiftHi & 0x00FF, 0x55);
}

// ==================== Rendering disabled ====================

TEST_F(PPUShiftRegisterTest, RenderingDisabled_NoShift) {
    // When rendering is disabled, shift registers should not change
    emulator.ppu().registerWrite(0x2001, 0x00);

    regs().bgPatternShiftLo = 0x1234;
    regs().bgPatternShiftHi = 0x5678;
    regs().bgAttrShiftLo = 0x9ABC;
    regs().bgAttrShiftHi = 0xDEF0;

    stepPPU(10);

    EXPECT_EQ(regs().bgPatternShiftLo, 0x1234);
    EXPECT_EQ(regs().bgPatternShiftHi, 0x5678);
    EXPECT_EQ(regs().bgAttrShiftLo, 0x9ABC);
    EXPECT_EQ(regs().bgAttrShiftHi, 0xDEF0);
}

// ==================== Multiple tiles ====================

TEST_F(PPUShiftRegisterTest, Cycle9_StartSecondTileFetch) {
    // Cycle 9 starts fetching the second tile
    regs().bgPatternShiftLo = 0x0001;
    regs().bgPatternShiftHi = 0x0001;
    stepPPU(10);
    EXPECT_EQ(regs().bgPatternShiftLo & 0xFE00, 0x0200);
    EXPECT_EQ(regs().bgPatternShiftHi & 0xFE00, 0x0200);
}
