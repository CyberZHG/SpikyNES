#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "test_ppu_utils.h"

std::string saveFrameBuffer(const PPU& ppu) {
    const auto* info = testing::UnitTest::GetInstance()->current_test_info();
    const std::filesystem::path suiteDir = info->test_suite_name();
    std::filesystem::create_directories(suiteDir);
    const std::filesystem::path filepath = suiteDir / (std::string(info->name()) + ".ppm");
    writePPM(filepath, ppu.frameBuffer());
    return filepath;
}
