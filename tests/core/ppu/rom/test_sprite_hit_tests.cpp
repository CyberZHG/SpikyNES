#include <gtest/gtest.h>
#include <string>
#include <format>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class SpriteHitTestsRoms : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(SpriteHitTestsRoms, 01_basics) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/01.basics.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 50) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 02_alignment) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/02.alignment.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 03_corners) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/03.corners.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 04_flip) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/04.flip.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 05_left_clip) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/05.left_clip.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 06_right_edge) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/06.right_edge.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 07_screen_bottom) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/07.screen_bottom.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 08_double_height) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/08.double_height.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 35) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 09_timing_basics) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/09.timing_basics.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 80) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 10_timing_order) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/10.timing_order.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 80) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(SpriteHitTestsRoms, 11_edge_timing) {
    const std::string romPath = "../tests/roms/ppu/sprite_hit_tests/11.edge_timing.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 80) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}