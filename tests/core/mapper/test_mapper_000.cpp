#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

class Mapper000Test : public testing::Test {
protected:
    Cartridge cartridge{nullptr};
};

TEST_F(Mapper000Test, PrgRom16KB_Mirroring) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    for (std::uint16_t offset = 0; offset < 0x4000; offset += 0x100) {
        EXPECT_EQ(cartridge.cpuRead(0x8000 + offset), cartridge.cpuRead(0xC000 + offset));
    }
}

TEST_F(Mapper000Test, PrgRom32KB_NoMirroring) {
    cartridge.initPrgRom(PRG_ROM_SIZE);
    cartridge.initMapper(0, 2, 0, 0);

    const auto val1 = cartridge.cpuRead(0x8000);
    const auto val2 = cartridge.cpuRead(0xC000);

    EXPECT_TRUE(val1 == 0x00 && val2 == 0x00);
}

TEST_F(Mapper000Test, PrgRom_FullAddressRange) {
    cartridge.initPrgRom(PRG_ROM_SIZE);
    cartridge.initMapper(0, 2, 0, 0);

    for (std::uint16_t addr = 0x8000; addr < 0x8100; ++addr) {
        cartridge.cpuWrite(addr, static_cast<std::uint8_t>(addr & 0xFF));
    }

    for (std::uint16_t addr = 0x8000; addr < 0x8100; ++addr) {
        EXPECT_EQ(cartridge.cpuRead(addr), 0x00);
    }
}

TEST_F(Mapper000Test, PrgRam_ReadWrite) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initPrgRam();
    cartridge.initMapper(0, 1, 1, 0);

    cartridge.cpuWrite(0x6000, 0xAA);
    cartridge.cpuWrite(0x7000, 0xBB);
    cartridge.cpuWrite(0x7FFF, 0xCC);

    EXPECT_EQ(cartridge.cpuRead(0x6000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0x7000), 0xBB);
    EXPECT_EQ(cartridge.cpuRead(0x7FFF), 0xCC);
}

TEST_F(Mapper000Test, PrgRam_NoRam_ReturnsZero) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    cartridge.cpuWrite(0x6000, 0xFF);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x00);
}

TEST_F(Mapper000Test, PrgRam_WritesIgnoredWithoutRam) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    cartridge.cpuWrite(0x6000, 0x42);
    cartridge.cpuWrite(0x7FFF, 0x99);

    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x00);
    EXPECT_EQ(cartridge.cpuRead(0x7FFF), 0x00);
}

TEST_F(Mapper000Test, ChrRom_ReadOnly) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 1);

    cartridge.ppuWrite(0x0000, 0x11);
    cartridge.ppuWrite(0x1000, 0x22);
    cartridge.ppuWrite(0x1FFF, 0x33);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x00);
}

TEST_F(Mapper000Test, ChrRam_NoChr_ReturnsZero) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x00);
}

TEST_F(Mapper000Test, ChrRam_ReadWrite) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    for (std::uint16_t addr = 0x0000; addr < 0x0100; ++addr) {
        cartridge.ppuWrite(addr, static_cast<std::uint8_t>(addr & 0xFF));
    }

    for (std::uint16_t addr = 0x0000; addr < 0x0100; ++addr) {
        EXPECT_EQ(cartridge.ppuRead(addr), static_cast<std::uint8_t>(addr & 0xFF));
    }
}

TEST_F(Mapper000Test, AddressBoundaries_Cpu) {
    cartridge.initPrgRom(PRG_ROM_SIZE);
    cartridge.initPrgRam();
    cartridge.initMapper(0, 2, 1, 0);

    cartridge.cpuWrite(0x5FFF, 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0x5FFF), 0x00);

    cartridge.cpuWrite(0x6000, 0xBB);
    EXPECT_EQ(cartridge.cpuRead(0x6000), 0xBB);

    cartridge.cpuWrite(0x7FFF, 0xCC);
    EXPECT_EQ(cartridge.cpuRead(0x7FFF), 0xCC);

    cartridge.cpuWrite(0x8000, 0xDD);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
}

TEST_F(Mapper000Test, AddressBoundaries_Ppu) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    cartridge.ppuWrite(0x0000, 0x11);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x11);

    cartridge.ppuWrite(0x1FFF, 0x22);
    EXPECT_EQ(cartridge.ppuRead(0x1FFF), 0x22);

    cartridge.ppuWrite(0x2000, 0x33);
    EXPECT_EQ(cartridge.ppuRead(0x2000), 0x00);
}

TEST_F(Mapper000Test, Independent_PrgRom_And_PrgRam) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initPrgRam();
    cartridge.initMapper(0, 1, 1, 0);

    cartridge.cpuWrite(0x6000, 0xAA);
    cartridge.cpuWrite(0x8000, 0xBB);

    EXPECT_EQ(cartridge.cpuRead(0x6000), 0xAA);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
    EXPECT_NE(cartridge.cpuRead(0x6000), cartridge.cpuRead(0x8000));
}

TEST_F(Mapper000Test, PrgRom16KB_MirrorPattern) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    for (std::uint16_t offset = 0; offset < 0x1000; offset += 0x100) {
        EXPECT_EQ(cartridge.cpuRead(0x8000 + offset), cartridge.cpuRead(0xC000 + offset));
    }
}

TEST_F(Mapper000Test, PrgRom_ReadOnly) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initMapper(0, 1, 0, 0);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
    EXPECT_EQ(cartridge.cpuRead(0xFFFF), 0x00);

    cartridge.cpuWrite(0x8000, 0x42);
    cartridge.cpuWrite(0xFFFF, 0x99);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
    EXPECT_EQ(cartridge.cpuRead(0xFFFF), 0x00);
}

TEST_F(Mapper000Test, ChrRom_vs_ChrRam) {
    cartridge.initPrgRom(PRG_ROM_BANK_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);

    cartridge.initMapper(0, 1, 0, 1);
    cartridge.ppuWrite(0x1000, 0xAA);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x00);

    cartridge.initMapper(0, 1, 0, 0);
    cartridge.ppuWrite(0x1000, 0xBB);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0xBB);
}

TEST_F(Mapper000Test, MultipleConfigurations) {
    cartridge.initPrgRom(PRG_ROM_SIZE);
    cartridge.initPrgRam(PRG_RAM_SIZE);
    cartridge.initChrRom(CHR_ROM_BANK_SIZE);
    cartridge.initMapper(0, 2, 1, 0);

    cartridge.cpuWrite(0x6000, 0x01);
    cartridge.cpuWrite(0x8000, 0x02);
    cartridge.cpuWrite(0xC000, 0x03);
    cartridge.ppuWrite(0x0000, 0x04);
    cartridge.ppuWrite(0x1000, 0x05);

    EXPECT_EQ(cartridge.cpuRead(0x6000), 0x01);
    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x00);
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0x00);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x04);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x05);
}
