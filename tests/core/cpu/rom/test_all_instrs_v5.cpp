#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../../ppu/test_ppu_utils.h"

using namespace nes_core;

class AllInstructions : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(AllInstructions, all_instrs_v5) {
    const std::string romPath = "../tests/roms/cpu/all_instrs.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 2500) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
