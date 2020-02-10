#include <gtest/gtest.h>
#include "test_ppu_utils.h"
#include "core/emulator.h"

using namespace nes_core;

class PPUBackgroundRenderingTest : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override {
        emulator.cartridge().initCHR(CHR_ROM_BANK_SIZE);
        emulator.reset();
        emulator.ppu().registerWrite(0x2001, 0x0A);
    }

    void stepPPU(const int cycles) {
        for (int i = 0; i < cycles; ++i) {
            emulator.ppu().step();
        }
    }

    void stepToScanline(const int targetScanline) {
        while (emulator.ppu().scanline() != targetScanline || emulator.ppu().cycle() != 0) {
            emulator.ppu().step();
        }
    }

    void renderOneFrame() {
        for (int i = 0; i < 262; ++i) {
            stepPPU(341);
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

    void writeNametable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().ppuWrite(addr, value);
    }

    void writeAttributeTable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().ppuWrite(addr, value);
    }

    void writePatternTable(const std::uint16_t addr, const std::uint8_t value) {
        emulator.ppu().ppuWrite(addr, value);
    }

    void writePalette(const std::uint8_t index, const std::uint8_t color) {
        emulator.ppu().ppuWrite(0x3F00 + index, color);
    }

    void fillNametable(const std::uint8_t tileIndex) {
        for (int i = 0; i < 960; ++i) {
            writeNametable(0x2000 + i, tileIndex);
        }
    }

    void fillAttributeTable(const std::uint8_t attr) {
        for (int i = 0; i < 64; ++i) {
            writeAttributeTable(0x23C0 + i, attr);
        }
    }

    void setupSolidTile(const std::uint8_t tileIndex, const std::uint8_t lowByte, const std::uint8_t highByte) {
        const std::uint16_t base = tileIndex * 16;
        for (int row = 0; row < 8; ++row) {
            writePatternTable(base + row, lowByte);
            writePatternTable(base + row + 8, highByte);
        }
    }
};

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_AllPixelsSameColor) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    fillNametable(0x00);
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0xFF, 0xFF);
    renderOneFrame();
    renderOneFrame();
    const auto firstPixel = getPixel(0, 0);
    for (int x = 0; x < 256; ++x) {
        EXPECT_EQ(getPixel(x, 0), firstPixel);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_BackgroundColor) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    fillNametable(0x00);
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0x00, 0x00);
    writePalette(0x00, 0x0F);
    renderOneFrame();
    renderOneFrame();
    const auto firstPixel = getPixel(0, 0);
    for (int x = 0; x < 256; ++x) {
        EXPECT_EQ(getPixel(x, 0), firstPixel);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_AlternatingPatternHorizontal) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    fillNametable(0x00);
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0xAA, 0x55);
    writePalette(0x00, 0x0F);
    writePalette(0x01, 0x11);
    writePalette(0x02, 0x21);
    renderOneFrame();
    renderOneFrame();
    const auto pixel0 = getPixel(0, 0);
    const auto pixel1 = getPixel(1, 0);
    EXPECT_NE(pixel0, pixel1);
    for (int x = 0; x < 8; x += 2) {
        EXPECT_EQ(getPixel(x, 0), pixel0);
        EXPECT_EQ(getPixel(x + 1, 0), pixel1);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_TileBoundary) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    writeNametable(0x2000, 0x00);
    writeNametable(0x2001, 0x01);
    for (int i = 2; i < 32; ++i) {
        writeNametable(0x2000 + i, 0x00);
    }
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0xFF, 0x00);
    setupSolidTile(0x01, 0x00, 0xFF);
    writePalette(0x01, 0x16);
    writePalette(0x02, 0x27);
    renderOneFrame();
    renderOneFrame();
    const auto color1 = getPixel(0, 0);
    for (int x = 0; x < 8; ++x) {
        EXPECT_EQ(getPixel(x, 0), color1);
    }
    const auto color2 = getPixel(8, 0);
    EXPECT_NE(color1, color2);
    for (int x = 8; x < 16; ++x) {
        EXPECT_EQ(getPixel(x, 0), color2);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_DifferentPalettes) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    fillNametable(0x00);
    setupSolidTile(0x00, 0xFF, 0x00);
    writeAttributeTable(0x23C0, 0b11100100);
    writePalette(0x01, 0x11);
    writePalette(0x05, 0x21);
    writePalette(0x09, 0x31);
    writePalette(0x0D, 0x01);
    renderOneFrame();
    renderOneFrame();
    const auto palette0Color = getPixel(0, 0);
    for (int x = 0; x < 16; ++x) {
        EXPECT_EQ(getPixel(x, 0), palette0Color);
    }
    const auto palette1Color = getPixel(16, 0);
    EXPECT_NE(palette0Color, palette1Color);
    for (int x = 16; x < 32; ++x) {
        EXPECT_EQ(getPixel(x, 0), palette1Color);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_VerticalStripes) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    for (int i = 0; i < 32; ++i) {
        writeNametable(0x2000 + i, i % 2);
    }
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0xFF, 0x00);
    setupSolidTile(0x01, 0x00, 0xFF);
    writePalette(0x01, 0x16);
    writePalette(0x02, 0x27);
    renderOneFrame();
    renderOneFrame();
    const auto stripe1 = getPixel(0, 0);
    const auto stripe2 = getPixel(8, 0);
    EXPECT_NE(stripe1, stripe2);
    for (int tile = 0; tile < 32; ++tile) {
        std::uint32_t expected = (tile % 2 == 0) ? stripe1 : stripe2;
        for (int x = 0; x < 8; ++x) {
            EXPECT_EQ(getPixel(tile * 8 + x, 0), expected);
        }
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_HorizontalStripes) {
    regs().v = 0x0000;
    regs().t = 0x0000;
    regs().x = 0;
    fillNametable(0x00);
    fillAttributeTable(0x00);
    for (int row = 0; row < 8; ++row) {
        if (row % 2 == 0) {
            writePatternTable(0x0000 + row, 0xFF);
            writePatternTable(0x0008 + row, 0x00);
        } else {
            writePatternTable(0x0000 + row, 0x00);
            writePatternTable(0x0008 + row, 0xFF);
        }
    }
    writePalette(0x01, 0x16);
    writePalette(0x02, 0x27);
    renderOneFrame();
    renderOneFrame();
    const auto color1 = getPixel(0, 0);
    const auto color2 = getPixel(0, 1);
    EXPECT_NE(color1, color2);
    for (int y = 0; y < 8; ++y) {
        std::uint32_t expected = (y % 2 == 0) ? color1 : color2;
        EXPECT_EQ(getPixel(0, y), expected);
    }
    saveFrameBuffer(emulator.ppu());
}

TEST_F(PPUBackgroundRenderingTest, FrameBuffer_CoarseXScroll) {
    regs().t = 0x0000;
    regs().x = 0;
    writeNametable(0x2000, 0x00);
    writeNametable(0x2001, 0x01);
    for (int i = 2; i < 32; ++i) {
        writeNametable(0x2000 + i, 0x00);
    }
    fillAttributeTable(0x00);
    setupSolidTile(0x00, 0xFF, 0x00);
    setupSolidTile(0x01, 0x00, 0xFF);
    regs().v = 0x0001;
    renderOneFrame();
    renderOneFrame();
    const auto tileColor = getPixel(0, 0);
    for (int x = 0; x < 8; ++x) {
        EXPECT_EQ(getPixel(x, 0), tileColor);
    }
    saveFrameBuffer(emulator.ppu());
}
