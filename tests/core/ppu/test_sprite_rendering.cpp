#include <gtest/gtest.h>
#include "test_ppu_utils.h"
#include "core/emulator.h"

using namespace nes_core;

class PPUSpriteRenderingTest : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initCHR(CHR_ROM_BANK_SIZE);
        emulator.reset();
        emulator.ppu().registerWrite(0x2001, 0x1E);
    }

    void stepPPU(const int cycles) {
        for (int i = 0; i < cycles; ++i) {
            emulator.ppu().step();
        }
    }

    void renderOneFrame() {
        for (int i = 0; i < 262; ++i) {
            stepPPU(341);
        }
    }

    void stepToScanline(const int targetScanline) {
        while (emulator.ppu().scanline() != targetScanline || emulator.ppu().cycle() != 0) {
            emulator.ppu().step();
        }
    }

    PPURegisters& regs() {
        return emulator.ppu().registers();
    }

    const std::array<std::uint32_t, PPU_FRAME_BUFFER_SIZE>& frameBuffer() {
        return emulator.ppu().frameBuffer();
    }

    std::uint32_t getPixel(const int x, const int y) {
        return frameBuffer()[y * 256 + x];
    }

    void writeOAM(const int index, const std::uint8_t y, const std::uint8_t tile, const std::uint8_t attr, const std::uint8_t x) {
        emulator.ppu().registerWrite(0x2003, index * 4);
        emulator.ppu().registerWrite(0x2004, y);
        emulator.ppu().registerWrite(0x2004, tile);
        emulator.ppu().registerWrite(0x2004, attr);
        emulator.ppu().registerWrite(0x2004, x);
    }

    void writePatternTable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().ppuWrite(addr, value);
    }

    void writePalette(const std::uint8_t index, const std::uint8_t color) {
        emulator.ppu().ppuWrite(0x3F00 + index, color);
    }

    void setupSolidSpriteTile(const std::uint8_t tileIndex, const std::uint8_t lowByte, const std::uint8_t highByte) {
        const std::uint16_t base = tileIndex * 16;
        for (int row = 0; row < 8; ++row) {
            writePatternTable(base + row, lowByte);
            writePatternTable(base + row + 8, highByte);
        }
    }

    void clearOAM() {
        for (int i = 0; i < 64; ++i) {
            writeOAM(i, 0xFF, 0, 0, 0);
        }
    }

    void fillBackgroundSolid(const std::uint8_t patternLo, const std::uint8_t patternHi) {
        for (int i = 0; i < 960; ++i) {
            emulator.ppu().ppuWrite(0x2000 + i, 0x01);
        }
        for (int i = 0; i < 64; ++i) {
            emulator.ppu().ppuWrite(0x23C0 + i, 0x00);
        }
        const std::uint16_t base = 0x01 * 16;
        for (int row = 0; row < 8; ++row) {
            writePatternTable(base + row, patternLo);
            writePatternTable(base + row + 8, patternHi);
        }
    }
};

TEST_F(PPUSpriteRenderingTest, SingleSprite) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    writeOAM(0, 10, 0x00, 0x00, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();
    const auto bgColor = getPixel(0, 0);
    const auto spriteColor = getPixel(20, 11);
    EXPECT_NE(bgColor, spriteColor);
    for (int x = 20; x < 28; ++x) {
        for (int y = 11; y < 19; ++y) {
            EXPECT_EQ(getPixel(x, y), spriteColor);
        }
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpritePriority) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    setupSolidSpriteTile(0x01, 0x00, 0xFF);
    writePalette(0x11, 0x16);
    writePalette(0x15, 0x27);
    writePalette(0x16, 0x09);
    writeOAM(0, 10, 0x00, 0x00, 20);
    writeOAM(1, 14, 0x01, 0x01, 24);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();
    const auto sprite0Color = getPixel(20, 11);
    for (int x = 20; x < 28; ++x) {
        for (int y = 11; y < 19; ++y) {
            EXPECT_EQ(getPixel(x, y), sprite0Color);
        }
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteInFrontOfBackground) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0xFF);
    writePalette(0x01, 0x11);
    writePalette(0x13, 0x27);
    writeOAM(0, 10, 0x00, 0x00, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();
    const auto bgColor = getPixel(0, 11);
    const auto spriteColor = getPixel(20, 11);
    EXPECT_NE(bgColor, spriteColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteBehindBackground) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0xFF);
    writePalette(0x01, 0x11);
    writePalette(0x13, 0x27);
    writeOAM(0, 10, 0x00, 0x20, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();
    const auto bgColor = getPixel(40, 11);
    const auto spriteColor = getPixel(20, 11);
    EXPECT_EQ(bgColor, spriteColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteBehindBackground_TransparentBG) {
    clearOAM();
    fillBackgroundSolid(0x00, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0xFF);
    writePalette(0x00, 0x0F);
    writePalette(0x13, 0x27);
    writeOAM(0, 10, 0x00, 0x20, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();

    const auto bgColor = getPixel(0, 11);
    const auto spriteColor = getPixel(20, 11);
    EXPECT_NE(bgColor, spriteColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, Sprite0Hit_Detected) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x01, 0x11);
    writePalette(0x11, 0x16);
    writeOAM(0, 8, 0x00, 0x00, 8);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;

    EXPECT_FALSE(regs().sprite0Hit());
    renderOneFrame();
    stepToScanline(20);
    EXPECT_TRUE(regs().sprite0Hit());
}

TEST_F(PPUSpriteRenderingTest, Sprite0Hit_NotAtX255) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x01, 0x11);
    writePalette(0x11, 0x16);
    writeOAM(0, 8, 0x00, 0x00, 248);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;

    renderOneFrame();
    stepToScanline(20);
    EXPECT_TRUE(regs().sprite0Hit());
}

TEST_F(PPUSpriteRenderingTest, Sprite0Hit_TransparentBG_NoHit) {
    clearOAM();
    fillBackgroundSolid(0x00, 0x00);
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    writeOAM(0, 8, 0x00, 0x00, 8);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;

    renderOneFrame();
    stepToScanline(20);
    EXPECT_FALSE(regs().sprite0Hit());
}

TEST_F(PPUSpriteRenderingTest, Sprite0Hit_TransparentSprite_NoHit) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0x00, 0x00);
    writePalette(0x01, 0x11);
    writeOAM(0, 8, 0x00, 0x00, 8);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;

    renderOneFrame();
    stepToScanline(20);
    EXPECT_FALSE(regs().sprite0Hit());
}

TEST_F(PPUSpriteRenderingTest, Sprite0Hit_OnlySprite0Triggers) {
    clearOAM();
    fillBackgroundSolid(0xFF, 0x00);
    setupSolidSpriteTile(0x00, 0x00, 0x00);
    setupSolidSpriteTile(0x01, 0xFF, 0x00);
    writePalette(0x01, 0x11);
    writePalette(0x11, 0x16);
    writeOAM(0, 8, 0x00, 0x00, 8);
    writeOAM(1, 8, 0x01, 0x00, 16);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;

    renderOneFrame();
    stepToScanline(20);
    EXPECT_FALSE(regs().sprite0Hit());
}

TEST_F(PPUSpriteRenderingTest, SpriteHorizontalFlip) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0x0F, 0x00);
    writePalette(0x11, 0x16);
    writeOAM(0, 10, 0x00, 0x00, 20);
    writeOAM(1, 20, 0x00, 0x40, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();

    const auto bgColor = getPixel(0, 0);
    const auto spriteColor = getPixel(24, 11);
    EXPECT_EQ(getPixel(20, 11), bgColor);
    EXPECT_EQ(getPixel(24, 11), spriteColor);
    EXPECT_EQ(getPixel(20, 21), spriteColor);
    EXPECT_EQ(getPixel(24, 21), bgColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteVerticalFlip) {
    clearOAM();
    for (int row = 0; row < 8; ++row) {
        writePatternTable(row, row == 0 ? 0xFF : 0x00);
        writePatternTable(row + 8, 0x00);
    }
    writePalette(0x11, 0x16);
    writeOAM(0, 10, 0x00, 0x00, 20);
    writeOAM(1, 20, 0x00, 0x80, 20);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();

    const auto bgColor = getPixel(0, 0);
    const auto spriteColor = getPixel(20, 11);
    EXPECT_NE(bgColor, spriteColor);
    EXPECT_EQ(getPixel(20, 11), spriteColor);
    EXPECT_EQ(getPixel(20, 12), bgColor);
    EXPECT_EQ(getPixel(20, 27), bgColor);
    EXPECT_EQ(getPixel(20, 28), spriteColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteDifferentPalettes) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    writePalette(0x15, 0x27);
    writePalette(0x19, 0x38);
    writePalette(0x1D, 0x09);
    writeOAM(0, 10, 0x00, 0x00, 20);
    writeOAM(1, 10, 0x00, 0x01, 40);
    writeOAM(2, 10, 0x00, 0x02, 60);
    writeOAM(3, 10, 0x00, 0x03, 80);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    renderOneFrame();
    renderOneFrame();

    const auto pal0 = getPixel(20, 11);
    const auto pal1 = getPixel(40, 11);
    const auto pal2 = getPixel(60, 11);
    const auto pal3 = getPixel(80, 11);
    EXPECT_NE(pal0, pal1);
    EXPECT_NE(pal1, pal2);
    EXPECT_NE(pal2, pal3);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteOverflow_MoreThan8Sprites) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    for (int i = 0; i < 9; ++i) {
        writeOAM(i, 10, 0x00, 0x00, 20 + i * 10);
    }
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    EXPECT_FALSE(regs().spriteOverflow());
    renderOneFrame();
    stepToScanline(20);
    EXPECT_TRUE(regs().spriteOverflow());
}

TEST_F(PPUSpriteRenderingTest, SpriteLeftClipping_Disabled) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    writeOAM(0, 10, 0x00, 0x00, 0);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    emulator.ppu().registerWrite(0x2001, 0x1E);
    renderOneFrame();
    renderOneFrame();
    const auto bgColor = getPixel(100, 11);
    const auto spriteColor = getPixel(0, 11);
    EXPECT_NE(bgColor, spriteColor);
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUSpriteRenderingTest, SpriteLeftClipping_Enabled) {
    clearOAM();
    setupSolidSpriteTile(0x00, 0xFF, 0x00);
    writePalette(0x11, 0x16);
    writeOAM(0, 10, 0x00, 0x00, 0);
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    emulator.ppu().registerWrite(0x2001, 0x1A);
    renderOneFrame();
    renderOneFrame();
    const auto bgColor = getPixel(100, 11);
    for (int x = 0; x < 8; ++x) {
        EXPECT_EQ(getPixel(x, 11), bgColor);
    }
    saveFrameBuffer(emulator.ppu());
}
