#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class PPUOpenBus : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(PPUOpenBus, ppu_open_bus) {
    const std::string romPath = "../tests/roms/ppu/ppu_open_bus.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 320) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
