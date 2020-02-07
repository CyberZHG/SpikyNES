#include <gtest/gtest.h>
#include "core/utils.h"

using namespace nes_core;

TEST(UtilsTest, WritePGM_ReadPGM_Binary) {
    constexpr int width = 4;
    constexpr int height = 3;
    const std::vector<std::uint8_t> original = {
        0, 64, 128, 255,
        32, 96, 160, 224,
        16, 48, 80, 112
    };
    const std::string path = "test_pgm_binary.pgm";
    ASSERT_TRUE(writePGM(path, width, height, original.data(), true));
    int readWidth, readHeight;
    const auto result = readPGM(path, readWidth, readHeight);
    EXPECT_EQ(readWidth, width);
    EXPECT_EQ(readHeight, height);
    EXPECT_EQ(result, original);
    std::remove(path.c_str());
}

TEST(UtilsTest, WritePGM_ReadPGM_ASCII) {
    constexpr int width = 4;
    constexpr int height = 3;
    const std::vector<std::uint8_t> original = {
        0, 64, 128, 255,
        32, 96, 160, 224,
        16, 48, 80, 112
    };
    const std::string path = "test_pgm_ascii.pgm";
    ASSERT_TRUE(writePGM(path, width, height, original.data(), false));
    int readWidth, readHeight;
    const auto result = readPGM(path, readWidth, readHeight);
    EXPECT_EQ(readWidth, width);
    EXPECT_EQ(readHeight, height);
    EXPECT_EQ(result, original);
    std::remove(path.c_str());
}

TEST(UtilsTest, WritePPM_ReadPPM_Binary) {
    constexpr int width = 2;
    constexpr int height = 2;
    const std::vector<std::uint8_t> original = {
        255, 0, 0,  0, 255, 0,
        0, 0, 255,  255, 255, 255
    };
    const std::string path = "test_ppm_binary.ppm";
    ASSERT_TRUE(writePPM(path, width, height, original.data(), true));
    int readWidth, readHeight;
    const auto result = readPPM(path, readWidth, readHeight);
    EXPECT_EQ(readWidth, width);
    EXPECT_EQ(readHeight, height);
    EXPECT_EQ(result, original);
    std::remove(path.c_str());
}

TEST(UtilsTest, WritePPM_ReadPPM_ASCII) {
    constexpr int width = 2;
    constexpr int height = 2;
    const std::vector<std::uint8_t> original = {
        255, 0, 0,  0, 255, 0,
        0, 0, 255,  255, 255, 255
    };
    const std::string path = "test_ppm_ascii.ppm";
    ASSERT_TRUE(writePPM(path, width, height, original.data(), false));
    int readWidth, readHeight;
    const auto result = readPPM(path, readWidth, readHeight);
    EXPECT_EQ(readWidth, width);
    EXPECT_EQ(readHeight, height);
    EXPECT_EQ(result, original);
    std::remove(path.c_str());
}
