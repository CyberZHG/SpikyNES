#include <gtest/gtest.h>
#include <string>

#include "core/emulator.h"
#include "../../ppu/test_ppu_utils.h"

using namespace nes_core;

class MMC3TestRoms : public testing::Test {
protected:
    Emulator emulator;

    void runTest(const std::string& romPath) {
        if (!emulator.loadROM(romPath)) {
            return;
        }
        while (emulator.ppu().frame() < 200) {
            emulator.step();
        }
        saveFrameBuffer(emulator.ppu());
    }
};

TEST_F(MMC3TestRoms, 1_Clocking) {
    runTest("../tests/roms/mapper/mmc3/1-clocking.nes");
}

TEST_F(MMC3TestRoms, 2_Details) {
    runTest("../tests/roms/mapper/mmc3/2-details.nes");
}

TEST_F(MMC3TestRoms, 3_A12_Clocking) {
    runTest("../tests/roms/mapper/mmc3/3-A12_clocking.nes");
}

TEST_F(MMC3TestRoms, 4_Scanline_Timing) {
    runTest("../tests/roms/mapper/mmc3/4-scanline_timing.nes");
}

TEST_F(MMC3TestRoms, 5_MMC3) {
    runTest("../tests/roms/mapper/mmc3/5-MMC3.nes");
}

// TEST_F(MMC3TestRoms, 6_MMC3_Alt) {
//     runTest("../tests/roms/mapper/mmc3/6-MMC3_alt.nes");
// }
