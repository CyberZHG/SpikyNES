#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

static void writeMMC1(Cartridge& cart, const std::uint16_t addr, const std::uint8_t value) {
    for (int i = 0; i < 5; ++i) {
        cart.cpuWrite(addr, (value >> i) & 1);
    }
}

class Mapper001Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};

    void setupPrgRom() {
        cartridge.initPrgRom(2 * PRG_ROM_BANK_SIZE);
        cartridge.cpuWrite(0x8000, 0xAA);
        cartridge.cpuWrite(0xC000, 0xBB);
    }

    void setupChrRom() {
        cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
        cartridge.ppuWrite(0x0000, 0xAA);
        cartridge.ppuWrite(0x1000, 0xBB);
    }
};

TEST_F(Mapper001Test, ShiftRegister_FiveWritesLoadRegister) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0x8000, 3);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::HORIZONTAL);
}

TEST_F(Mapper001Test, ShiftRegister_ResetOnBit7) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    cartridge.cpuWrite(0x8000, 1);
    cartridge.cpuWrite(0x8000, 0);
    cartridge.cpuWrite(0x8000, 0x80);

    writeMMC1(cartridge, 0x8000, 2);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::VERTICAL);
}

TEST_F(Mapper001Test, Control_AllMirroringModes) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0x8000, 0);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::SINGLE_LOWER);

    writeMMC1(cartridge, 0x8000, 1);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::SINGLE_UPPER);

    writeMMC1(cartridge, 0x8000, 2);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::VERTICAL);

    writeMMC1(cartridge, 0x8000, 3);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::HORIZONTAL);
}

TEST_F(Mapper001Test, PrgBanking_Mode3_IsDefault) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}

TEST_F(Mapper001Test, PrgBanking_Mode3_SwitchAt8000) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0xE000, 1);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}

TEST_F(Mapper001Test, PrgBanking_Mode2_FixFirstAt8000) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0x8000, 8);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xAA);

    writeMMC1(cartridge, 0xE000, 1);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}

TEST_F(Mapper001Test, PrgBanking_Mode0_IgnoresOddBankIndex) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0x8000, 0);
    writeMMC1(cartridge, 0xE000, 1);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);

    writeMMC1(cartridge, 0x8000, 12);
    writeMMC1(cartridge, 0xE000, 1);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xBB);
}

TEST_F(Mapper001Test, ChrBanking_8KB_Switch) {
    setupPrgRom();
    setupChrRom();
    cartridge.initMapper(1, 2, 0, 2);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0xBB);

    writeMMC1(cartridge, 0xA000, 2);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x00);
}

TEST_F(Mapper001Test, ChrBanking_4KB_Independent) {
    setupPrgRom();
    setupChrRom();
    cartridge.initMapper(1, 2, 0, 2);

    writeMMC1(cartridge, 0x8000, 28);
    writeMMC1(cartridge, 0xA000, 0);
    writeMMC1(cartridge, 0xC000, 1);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0xBB);

    writeMMC1(cartridge, 0xA000, 1);
    writeMMC1(cartridge, 0xC000, 0);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xBB);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0xAA);
}

TEST_F(Mapper001Test, PrgRam_ReadWrite) {
    setupPrgRom();
    cartridge.initPrgRam();
    cartridge.initMapper(1, 2, 1, 0);

    cartridge.cpuWrite(0x6000, 0x12);
    cartridge.cpuWrite(0x7FFF, 0x34);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x12);
    EXPECT_EQ(cartridge.cpuRead(0x7FFF), 0x34);
}

TEST_F(Mapper001Test, PrgRam_DisabledByMMC1B) {
    setupPrgRom();
    cartridge.initPrgRam();
    cartridge.initMapper(1, 2, 1, 0);

    cartridge.cpuWrite(0x6000, 0xAB);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0xAB);

    writeMMC1(cartridge, 0xE000, 0b10000);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x00);
}

TEST_F(Mapper001Test, Reset_RestoresMode3) {
    setupPrgRom();
    cartridge.initMapper(1, 2, 0, 0);

    writeMMC1(cartridge, 0x8000, 8);
    writeMMC1(cartridge, 0xE000, 1);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);

    cartridge.reset();
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0xBB);
}
