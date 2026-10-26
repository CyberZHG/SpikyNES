#include <gtest/gtest.h>
#include "core/cartridge.h"

using namespace nes_core;

class Mapper011Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};

    void setupRom() {
        cartridge.initPrgRom(2 * 0x8000);
        cartridge.cpuWrite(0x8000, 0xA1);
        cartridge.initChrRom(2 * 0x2000);
        cartridge.ppuWrite(0x0000, 0xC0);
    }
};

TEST_F(Mapper011Test, PrgRom_DefaultState) {
    setupRom();
    cartridge.initMapper(11, 2, 0, 2);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xA1);
}

TEST_F(Mapper011Test, PrgBanking_UsesLowBits) {
    setupRom();
    cartridge.initMapper(11, 2, 0, 2);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);

    cartridge.cpuWrite(0x8000, 0x10);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xA1);
}

TEST_F(Mapper011Test, ChrRom_DefaultState) {
    setupRom();
    cartridge.initMapper(11, 2, 0, 2);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xC0);
}

TEST_F(Mapper011Test, ChrBanking_UsesHighNibble) {
    setupRom();
    cartridge.initMapper(11, 2, 0, 2);

    cartridge.cpuWrite(0x8000, 0x10);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xC0);
}

TEST_F(Mapper011Test, PrgBanking_BusConflict) {
    cartridge.initPrgRom(2 * 0x8000);
    cartridge.cpuWrite(0x8000, 0x02);
    cartridge.header().isNES2 = true;
    cartridge.header().subMapper = 2;
    cartridge.initMapper(11, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x02);
}

TEST_F(Mapper011Test, ChrRam_ReadWrite) {
    cartridge.initPrgRom(2 * 0x8000);
    cartridge.initChrRam();
    cartridge.initMapper(11, 2, 0, 0);

    cartridge.ppuWrite(0x0000, 0x42);
    cartridge.ppuWrite(0x1FFF, 0x99);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x42);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x99);
}
