#ifndef SPIKYNES_TEST_CPU_UTILS_H
#define SPIKYNES_TEST_CPU_UTILS_H

#include <gtest/gtest.h>
#include "core/emulator.h"
using namespace nes_core;

class CPUTestBase : public testing::Test {
protected:
    Emulator emulator;

    void SetUp() override;
    int runProgram(const std::vector<std::uint8_t>& program, int steps);
};

#endif //SPIKYNES_TEST_CPU_UTILS_H