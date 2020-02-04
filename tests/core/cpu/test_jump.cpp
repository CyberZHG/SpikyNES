#include "test_cpu_utils.h"

// ==================== JMP ====================

TEST_F(CPUTestBase, JMP_Absolute) {
    // JMP $0300
    const auto cycles = runProgram({0x4C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0300);
}

TEST_F(CPUTestBase, JMP_Indirect) {
    // JMP ($0300) ; Jump to address stored at $0300
    emulator.cpu().write(0x0300, 0x50);
    emulator.cpu().write(0x0301, 0x04);
    const auto cycles = runProgram({0x6C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0450);
}

TEST_F(CPUTestBase, JMP_Indirect_PageBoundaryBug) {
    // JMP ($03FF) ; 6502 bug: high byte read from $0300 instead of $0400
    emulator.cpu().write(0x03FF, 0x50);
    emulator.cpu().write(0x0300, 0x04);
    const auto cycles = runProgram({0x6C, 0xFF, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0450);
}
