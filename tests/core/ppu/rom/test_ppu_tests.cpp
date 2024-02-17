#include <gtest/gtest.h>
#include <string>
#include <format>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class BlarggPPUTestsRoms : public testing::Test {
protected:
    Emulator emulator;

    static void compareWithExpectedOutput(const std::string& actualPath) {
        const std::string expectedPath = "../tests/roms/ppu/blargg_ppu_tests_expected.ppm";
        int expectedWidth = 0, expectedHeight = 0;
        const auto expected = readPPM(expectedPath, expectedWidth, expectedHeight);
        int actualWidth = 0, actualHeight = 0;
        const auto actual = readPPM(actualPath, actualWidth, actualHeight);
        ASSERT_EQ(actualWidth, expectedWidth);
        ASSERT_EQ(actualHeight, expectedHeight);
        EXPECT_EQ(actual, expected);
    }
};

TEST_F(BlarggPPUTestsRoms, PaletteRAM) {
    const std::string romPath = "../tests/roms/ppu/palette_ram.nes";
    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load palette_ram.nes";
    while (emulator.ppu().frame() < 30) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
    compareWithExpectedOutput(filepath);
}

TEST_F(BlarggPPUTestsRoms, SpriteRAM) {
    const std::string romPath = "../tests/roms/ppu/sprite_ram.nes";
    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load sprite_ram.nes";
    while (emulator.ppu().frame() < 30) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
    compareWithExpectedOutput(filepath);
}

TEST_F(BlarggPPUTestsRoms, VBLClearTime) {
    const std::string romPath = "../tests/roms/ppu/vbl_clear_time.nes";
    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load vbl_clear_time.nes";
    while (emulator.ppu().frame() < 30) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
    compareWithExpectedOutput(filepath);
}

TEST_F(BlarggPPUTestsRoms, VRAMAccess) {
    const std::string romPath = "../tests/roms/ppu/vram_access.nes";
    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load vram_access.nes";
    while (emulator.ppu().frame() < 30) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
    compareWithExpectedOutput(filepath);
}
