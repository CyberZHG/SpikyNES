#include <gtest/gtest.h>
#include "core/cartridge.h"
#include "core/constants.h"

using namespace nes_core;

class Mapper206BankingTest : public testing::Test {
protected:
    Cartridge cartridge{nullptr};

    void SetUp() override {
        // 32KB PRG = four 8KB banks; mark the start of each bank.
        cartridge.initPrgRom(2 * PRG_ROM_BANK_SIZE);
        cartridge.cpuWrite(0x8000, 0x10);  // bank 0
        cartridge.cpuWrite(0xA000, 0x20);  // bank 1
        cartridge.cpuWrite(0xC000, 0x30);  // bank 2 (second-to-last)
        cartridge.cpuWrite(0xE000, 0x40);  // bank 3 (last)

        // 16KB CHR = sixteen 1KB banks; mark a few bank boundaries.
        cartridge.initChrRom(2 * CHR_ROM_BANK_SIZE);
        cartridge.ppuWrite(0x0000, 0x0A);  // 1KB bank 0
        cartridge.ppuWrite(0x0800, 0x0B);  // 1KB bank 2
        cartridge.ppuWrite(0x1000, 0x0C);  // 1KB bank 4

        cartridge.initMapper(206, 2, 0, 2);
    }
};

TEST_F(Mapper206BankingTest, PrgMode0_R6SelectsFirstWindow) {
    cartridge.cpuWrite(0x8000, 6);
    cartridge.cpuWrite(0x8001, 1);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x20);
}

TEST_F(Mapper206BankingTest, PrgMode0_R7SelectsSecondWindow) {
    cartridge.cpuWrite(0x8000, 7);
    cartridge.cpuWrite(0x8001, 1);

    EXPECT_EQ(cartridge.cpuRead(0xA000), 0x20);
}

TEST_F(Mapper206BankingTest, PrgMode0_HighWindowsAreFixed) {
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0x30);
    EXPECT_EQ(cartridge.cpuRead(0xE000), 0x40);
}

// Bit 6 of the bank-select register has no effect on mapper 206 (no PRG mode inversion)
TEST_F(Mapper206BankingTest, PrgModeBitIgnored_LayoutStaysFixed) {
    cartridge.cpuWrite(0x8000, 0x46);  // R6 with bit 6 set
    cartridge.cpuWrite(0x8001, 1);

    EXPECT_EQ(cartridge.cpuRead(0x8000), 0x20);  // still R6 window
    EXPECT_EQ(cartridge.cpuRead(0xC000), 0x30);  // still fixed second-to-last
}

TEST_F(Mapper206BankingTest, ChrMode0_R0Selects2KBAt0000) {
    cartridge.cpuWrite(0x8000, 0);
    cartridge.cpuWrite(0x8001, 2);

    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x0B);
}

TEST_F(Mapper206BankingTest, ChrMode0_R2Selects1KBAt1000) {
    cartridge.cpuWrite(0x8000, 0x02);
    cartridge.cpuWrite(0x8001, 4);

    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x0C);
}

// Bit 7 of the bank-select register has no effect on mapper 206 (no CHR mode inversion)
TEST_F(Mapper206BankingTest, ChrModeBitIgnored_LayoutStaysFixed) {
    cartridge.cpuWrite(0x8000, 0x80);  // R0 with bit 7 set
    cartridge.cpuWrite(0x8001, 2);
    EXPECT_EQ(cartridge.ppuRead(0x0000), 0x0B);  // 2KB bank still at $0000

    cartridge.cpuWrite(0x8000, 0x82);  // R2 with bit 7 set
    cartridge.cpuWrite(0x8001, 4);
    EXPECT_EQ(cartridge.ppuRead(0x1000), 0x0C);  // 1KB bank still at $1000
}

// Mapper 206 has no mirroring register
TEST_F(Mapper206BankingTest, Mirroring_FromHeader) {
    cartridge.setMirroring(Mirroring::VERTICAL);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::VERTICAL);

    cartridge.setMirroring(Mirroring::HORIZONTAL);
    EXPECT_EQ(cartridge.mirroring(), Mirroring::HORIZONTAL);
}

TEST_F(Mapper206BankingTest, Mirroring_A000WriteIsIgnored) {
    cartridge.setMirroring(Mirroring::HORIZONTAL);
    cartridge.cpuWrite(0xA000, 0x00);  // would select vertical on MMC3

    EXPECT_EQ(cartridge.mirroring(), Mirroring::HORIZONTAL);
}
