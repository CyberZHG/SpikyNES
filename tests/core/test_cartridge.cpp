#include <gtest/gtest.h>
#include <fstream>

#include "core/emulator.h"

using namespace nes_core;

TEST(CartridgeTest, LoadInvalidFile) {
    Emulator emulator;
    EXPECT_FALSE(emulator.loadROM("nonexistent.nes"));
}

TEST(CartridgeTest, LoadInvalidFormat) {
    const std::string path = "invalid_test.nes";
    std::ofstream file(path, std::ios::binary);
    file << "INVALID_HEADER";
    file.close();
    Emulator emulator;
    EXPECT_FALSE(emulator.loadROM(path));
    std::remove(path.c_str());
}

TEST(CartridgeTest, LoadValidNES) {
    const std::string path = "valid_test.nes";
    std::ofstream file(path, std::ios::binary);
    file.put('N').put('E').put('S').put(0x1A);
    file.put(1);
    file.put(0);
    file.put(0);
    file.put(0);
    for (int i = 8; i < 16; ++i) {
        file.put(0);
    }
    for (int i = 0; i < 16384; ++i) {
        file.put(0xEA);
    }
    file.close();
    Emulator emulator;
    EXPECT_TRUE(emulator.loadROM(path));
    EXPECT_EQ(emulator.cartridge().header().prgRomBanks, 1);
    EXPECT_EQ(emulator.cartridge().header().chrRomBanks, 0);
    EXPECT_EQ(emulator.cartridge().header().mapper, 0);

    EXPECT_EQ(emulator.bus().cpuRead(0x8000), 0xEA);
    EXPECT_EQ(emulator.bus().cpuRead(0xC000), 0xEA);

    std::remove(path.c_str());
}

TEST(CartridgeTest, LoadNESWithResetVector) {
    const std::string path = "reset_test.nes";
    std::ofstream file(path, std::ios::binary);
    file.put('N').put('E').put('S').put(0x1A);
    file.put(1);
    file.put(0);
    file.put(0);
    file.put(0);
    for (int i = 8; i < 16; ++i) {
        file.put(0);
    }
    std::vector<uint8_t> prg(0x4000, 0xEA);
    prg[0x3FFC] = 0x00;
    prg[0x3FFD] = 0xC0;
    file.write(reinterpret_cast<char*>(prg.data()), prg.size());
    file.close();

    Emulator emulator;
    EXPECT_TRUE(emulator.loadROM(path));
    EXPECT_EQ(emulator.cpu().registers().PC, 0xC000);

    std::remove(path.c_str());
}
