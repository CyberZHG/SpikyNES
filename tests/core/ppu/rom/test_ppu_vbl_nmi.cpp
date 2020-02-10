#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"

using namespace nes_core;

class PPUVblNmiTest : public testing::Test {
protected:
    Emulator emulator;

    static constexpr std::uint16_t STATUS_ADDR = 0x6000;
    static constexpr std::uint16_t MAGIC_ADDR = 0x6001;
    static constexpr std::uint16_t TEXT_ADDR = 0x6004;

    static constexpr std::uint8_t STATUS_RUNNING = 0x80;
    static constexpr std::uint8_t STATUS_NEED_RESET = 0x81;
    static constexpr std::uint8_t STATUS_PASSED = 0x00;

    static constexpr std::uint8_t MAGIC_0 = 0xDE;
    static constexpr std::uint8_t MAGIC_1 = 0xB0;
    static constexpr std::uint8_t MAGIC_2 = 0x61;

    bool isMagicValid() const {
        return emulator.bus().cpuRead(MAGIC_ADDR) == MAGIC_0 &&
               emulator.bus().cpuRead(MAGIC_ADDR + 1) == MAGIC_1 &&
               emulator.bus().cpuRead(MAGIC_ADDR + 2) == MAGIC_2;
    }

    std::uint8_t getStatus() const {
        return emulator.bus().cpuRead(STATUS_ADDR);
    }

    std::string getOutputText() const {
        std::string text;
        for (std::uint16_t addr = TEXT_ADDR; addr < 0x6800; ++addr) {
            const auto c = emulator.bus().cpuRead(addr);
            if (c == 0) {
                break;
            }
            text += static_cast<char>(c);
        }
        return text;
    }
};

TEST_F(PPUVblNmiTest, RunPPUVblNmiTest) {
    const std::string romPath = "../tests/roms/ppu/ppu_vbl_nmi.nes";
    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load ppu_vbl_nmi.nes";
    while (emulator.ppu().frame() < 2000) {
        emulator.step();
        if (isMagicValid() && getStatus() != STATUS_RUNNING) {
            break;
        }
    }
    ASSERT_TRUE(isMagicValid()) << "Invalid magic bytes)";
    const auto status = getStatus();
    const auto output = getOutputText();
    EXPECT_EQ(status, STATUS_PASSED)
        << "Test failed: 0x" << std::hex << static_cast<int>(status) << "\n"
        << "Output:\n" << output;
}
