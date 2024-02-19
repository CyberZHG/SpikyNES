#include <gtest/gtest.h>
#include <string>
#include <format>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class SpriteOverflowTestsRoms : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(SpriteOverflowTestsRoms, 1_Basics) {
    const std::string romPath = "../tests/roms/ppu/sprite_overflow_tests/1.Basics.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteOverflowTestsRoms, 2_Details) {
    const std::string romPath = "../tests/roms/ppu/sprite_overflow_tests/2.Details.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteOverflowTestsRoms, 3_Timing) {
    const std::string romPath = "../tests/roms/ppu/sprite_overflow_tests/3.Timing.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 120) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteOverflowTestsRoms, 4_Obscure) {
    const std::string romPath = "../tests/roms/ppu/sprite_overflow_tests/4.Obscure.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteOverflowTestsRoms, 5_Emulator) {
    const std::string romPath = "../tests/roms/ppu/sprite_overflow_tests/5.Emulator.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
