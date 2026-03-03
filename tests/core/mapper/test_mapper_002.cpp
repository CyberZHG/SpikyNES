#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

class Mapper002Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};

    void setupPrgRom() {
        cartridge.initPrgRom(2 * PRG_ROM_BANK_SIZE);
        cartridge.cpuWrite(0x8000, 0x01);
        cartridge.cpuWrite(0xC000, 0xBB);
    }
};

TEST_F(Mapper002Test, PrgRom_DefaultState) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x01);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}

TEST_F(Mapper002Test, PrgBanking_SwitchAt8000) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);
}

TEST_F(Mapper002Test, PrgBanking_LastBankFixed) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);

    cartridge.cpuWrite(0x8000, 0x00);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x01);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}

TEST_F(Mapper002Test, PrgBanking_WritableFromAnyPrgAddress) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0xC000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);
}

TEST_F(Mapper002Test, PrgBanking_BusConflict) {
    cartridge.initPrgRom(2 * PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x02);
    cartridge.cpuWrite(0xC000, 0xBB);
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x02);
}

TEST_F(Mapper002Test, PrgBanking_BankMask) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0xFF);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);
}

TEST_F(Mapper002Test, ChrRam_ReadWrite) {
    setupPrgRom();
    cartridge.initChrRam();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.ppuWrite(0x0000, 0x42);
    cartridge.ppuWrite(0x1FFF, 0x99);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x42);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x99);
}

TEST_F(Mapper002Test, ChrRom_ReadOnly) {
    setupPrgRom();
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initMapper(2, 2, 0, 1);

    cartridge.ppuWrite(0x0000, 0xFF);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper002Test, Reset_RestoresBank0) {
    setupPrgRom();
    cartridge.initMapper(2, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);

    cartridge.reset();
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x01);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}
