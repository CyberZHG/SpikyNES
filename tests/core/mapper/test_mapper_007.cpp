#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

class Mapper007Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};

    void setupPrgRom() {
        cartridge.initPrgRom(2 * 0x8000);
        cartridge.cpuWrite(0x8000, 0x01);
    }
};

TEST_F(Mapper007Test, PrgRom_DefaultState) {
    setupPrgRom();
    cartridge.initMapper(7, 2, 0, 0);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x01);
}

TEST_F(Mapper007Test, PrgBanking_Switch) {
    setupPrgRom();
    cartridge.initMapper(7, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
}

TEST_F(Mapper007Test, PrgBanking_BusConflict) {
    setupPrgRom();
    cartridge.cpuWrite(0x8000, 0x10);
    cartridge.header().isNES2 = true;
    cartridge.header().subMapper = 2;
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x10);
}

TEST_F(Mapper007Test, ChrRam_ReadWrite) {
    setupPrgRom();
    cartridge.initChrRam();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.ppuWrite(0x0000, 0x42);
    cartridge.ppuWrite(0x1FFF, 0x99);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x42);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x99);
}

TEST_F(Mapper007Test, ChrRom_ReadOnly) {
    setupPrgRom();
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initMapper(2, 2, 0, 1);

    cartridge.ppuWrite(0x0000, 0xFF);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}
