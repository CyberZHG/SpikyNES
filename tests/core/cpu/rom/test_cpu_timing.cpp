#include <gtest/gtest.h>
#include <string>
#include <format>

#include "core/emulator.h"
#include "../../ppu/test_ppu_utils.h"

using namespace nes_core;

class CPUTimingTest : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(CPUTimingTest, DISABLED_cpu_timing_test) {
    const std::string romPath = "../tests/roms/cpu/cpu_timing_test.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    emulator.controller1().press(Controller::Button::B);
    while (emulator.ppu().frame() < 1000) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
