#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class VblNmiTimingRoms : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(VblNmiTimingRoms, 1_frame_basics) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/1.frame_basics.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 180) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 2_vbl_timing) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/2.vbl_timing.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 160) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 3_even_odd_frames) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/3.even_odd_frames.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 160) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 4_vbl_clear_timing) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/4.vbl_clear_timing.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 140) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 5_nmi_suppression) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/5.nmi_suppression.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 170) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 6_nmi_disable) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/6.nmi_disable.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 120) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}

TEST_F(VblNmiTimingRoms, 7_nmi_timing) {
    const std::string romPath = "../tests/roms/ppu/vbl_nmi_timing/7.nmi_timing.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 60) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
