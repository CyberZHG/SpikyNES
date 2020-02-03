#include "test_cpu_utils.h"
#include "core/constants.h"

void CPUTestBase::SetUp() {
    emulator.reset();
}

int CPUTestBase::runProgram(const std::vector<std::uint8_t>& program, const int steps) {
    emulator.cpu().registers().PC = CPU_PROGRAM_BEGIN;
    for (size_t i = 0; i < program.size(); ++i) {
        emulator.cpu().write(CPU_PROGRAM_BEGIN + i, program[i]);
    }
    int cycles = 0;
    for (int i = 0; i < steps; ++i) {
        cycles += emulator.cpu().step();
    }
    return cycles;
}

TEST_F(CPUTestBase, reset) {
    const auto cycles = runProgram({}, 0);
    EXPECT_EQ(cycles, 0);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00100100);
}

TEST_F(CPUTestBase, nop) {
    const auto cycles = runProgram({OPCODE_NOP}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00100100);
}
