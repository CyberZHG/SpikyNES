#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../../ppu/test_ppu_utils.h"

using namespace nes_core;

class APUTest : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(APUTest, apu_test) {
    const std::string romPath = "../tests/roms/apu/apu_test.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 180) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
