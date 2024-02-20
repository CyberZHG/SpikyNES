#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../test_ppu_utils.h"

using namespace nes_core;

class PPUReadBuffer : public testing::Test {
protected:
    Emulator emulator;
};

TEST_F(PPUReadBuffer, ppu_read_buffer) {
    const std::string romPath = "../tests/roms/ppu/test_ppu_read_buffer.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (emulator.ppu().frame() < 1300) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}
