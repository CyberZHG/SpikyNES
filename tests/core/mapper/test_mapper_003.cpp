#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

class Mapper003Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};
};

TEST_F(Mapper003Test, PrgRom16KB_Mirroring) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    for (std::uint16_t offset = 0; offset < 0x4000; offset += 0x100) {
        EXPECT_EQ(cartridge.cpuRead(0x8000 + offset), cartridge.cpuRead(0xC000 + offset));
    }
}

TEST_F(Mapper003Test, PrgRom32KB_NoMirroring) {
    cartridge.initPrgRom(PRG_ROM_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x11);
    cartridge.cpuWrite(0xC000, 0x22);
    cartridge.initMapper(3, 2, 0, 1);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x11);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0x22);
}

TEST_F(Mapper003Test, PrgRom_ReadOnly) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x42);
    cartridge.initMapper(3, 1, 0, 1);
    cartridge.cpuWrite(0x8000, 0x00);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x42);
}

TEST_F(Mapper003Test, PrgRam_ReadWrite) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initPrgRam();
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 1, 1);

    cartridge.cpuWrite(0x6000, 0xAA);
    cartridge.cpuWrite(0x7FFF, 0xBB);

    EXPECT_EQ(cartridge.cpuRead(0x6000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0x7FFF), 0xBB);
}

TEST_F(Mapper003Test, PrgRam_NoRam_ReturnsZero) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    cartridge.cpuWrite(0x6000, 0xFF);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x00);
}

TEST_F(Mapper003Test, ChrBank_DefaultIsBank0) {
    cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 2);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper003Test, ChrBankSwitch_SelectsCorrectBank) {
    cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x01);
    cartridge.initMapper(3, 1, 0, 2);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);

    cartridge.cpuWrite(0x8000, 0xFF);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);

    cartridge.cpuWrite(0x8000, 0xFE);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper003Test, ChrBankSwitch_WritableFromAnyPrgAddress) {
    cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x01);
    cartridge.cpuWrite(0xA000, 0x01);
    cartridge.initMapper(3, 1, 0, 2);

    cartridge.cpuWrite(0xA000, 0xFF);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
    cartridge.cpuWrite(0x8000, 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper003Test, ChrBankSwitch_BusConflict_PrgByteAndWriteValue) {
    cartridge.initChrRom(4 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x00);
    cartridge.initMapper(3, 1, 0, 4);

    cartridge.cpuWrite(0x8000, 0x03);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper003Test, ChrBankSwitch_BusConflict_16kbPrgMirror) {
    cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0x01);
    cartridge.initMapper(3, 1, 0, 2);

    cartridge.cpuWrite(0xC000, 0xFF);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
}

TEST_F(Mapper003Test, ChrBankSwitch_MaskClampsToValidRange) {
    cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0xFF);
    cartridge.initMapper(3, 1, 0, 2);

    cartridge.cpuWrite(0x8000, 0xFF);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
}

TEST_F(Mapper003Test, ChrRom_ReadOnly) {
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0xAA);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    cartridge.ppuWrite(0x0000, 0xFF);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0xAA);
}

TEST_F(Mapper003Test, ChrBank_FullAddressRange) {
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    for (std::uint16_t addr = 0x0000; addr < 0x2000; addr += 0x100) {
        cartridge.ppuWrite(addr, static_cast<std::uint8_t>(addr >> 8));
    }
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    for (std::uint16_t addr = 0x0000; addr < 0x2000; addr += 0x100) {
        EXPECT_EQ(cartridge.ppuRead(addr), static_cast<std::uint8_t>(addr >> 8));
    }
}

TEST_F(Mapper003Test, CpuRead_BelowPrgRam_ReturnsZero) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    EXPECT_EQ(cartridge.cpuRead(0x0000), 0x00);
    EXPECT_EQ(cartridge.cpuRead(0x5FFF), 0x00);
}

TEST_F(Mapper003Test, PpuRead_AboveChrRange_ReturnsZero) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(3, 1, 0, 1);

    EXPECT_EQ(cartridge.ppuRead(0x2000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x3FFF), 0x00);
}

TEST_F(Mapper003Test, ChrBankSwitch_4Banks) {
    cartridge.initChrRom(4 * CHR_ROM_BANK_SIZE);
    cartridge.ppuWrite(0x0000, 0x11);
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.cpuWrite(0x8000, 0xFF);
    cartridge.initMapper(3, 1, 0, 4);

    cartridge.cpuWrite(0x8000, 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x11);

    cartridge.cpuWrite(0x8000, 0x01);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);

    cartridge.cpuWrite(0x8000, 0x02);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);

    cartridge.cpuWrite(0x8000, 0x03);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);

    cartridge.cpuWrite(0x8000, 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x11);
}
