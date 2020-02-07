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
    emulator.cpu().cpuWrite(0x0300, 0x50);
    emulator.cpu().cpuWrite(0x0301, 0x04);
    const auto cycles = runProgram({0x6C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0450);
}

TEST_F(CPUTestBase, JMP_Indirect_PageBoundaryBug) {
    // JMP ($03FF) ; 6502 bug: high byte read from $0300 instead of $0400
    emulator.cpu().cpuWrite(0x03FF, 0x50);
    emulator.cpu().cpuWrite(0x0300, 0x04);
    const auto cycles = runProgram({0x6C, 0xFF, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0450);
}


// ==================== JSR ====================

TEST_F(CPUTestBase, JSR) {
    // JSR $0300 ; Push return address - 1, jump to $0300
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x20, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0300);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFD);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), (CPU_PROGRAM_BEGIN + 2) >> 8);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FE), (CPU_PROGRAM_BEGIN + 2) & 0xFF);
}

// ==================== RTS ====================

TEST_F(CPUTestBase, RTS) {
    // RTS ; Pull return address + 1, jump there
    emulator.cpu().registers().SP = 0xFD;
    emulator.cpu().cpuWrite(0x01FE, 0xFF);  // Low byte
    emulator.cpu().cpuWrite(0x01FF, 0x02);  // High byte ($02FF + 1 = $0300)
    const auto cycles = runProgram({0x60}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0300);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}

TEST_F(CPUTestBase, JSR_RTS_RoundTrip) {
    // JSR then RTS should return to instruction after JSR
    // Program: JSR $0210, NOP (at $0210: RTS)
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().cpuWrite(0x0210, 0x60);  // RTS at $0210
    const auto cycles = runProgram({0x20, 0x10, 0x02, 0xEA}, 3);
    // JSR(6) + RTS(6) + NOP(2) = 14 cycles
    EXPECT_EQ(cycles, 14);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}
