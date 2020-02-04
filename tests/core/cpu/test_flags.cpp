#include "test_cpu_utils.h"

// ==================== CLC ====================

TEST_F(CPUTestBase, CLC) {
    // CLC ; Clear Carry
    emulator.cpu().registers().P |= 0x01;  // Set C
    const auto cycles = runProgram({0x18}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, CLC_AlreadyClear) {
    // CLC ; Already clear
    emulator.cpu().registers().P &= ~0x01;  // Clear C
    const auto cycles = runProgram({0x18}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

// ==================== CLD ====================

TEST_F(CPUTestBase, CLD) {
    // CLD ; Clear Decimal
    emulator.cpu().registers().P |= 0x08;  // Set D
    const auto cycles = runProgram({0xD8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().P & 0x08, 0);
}

// ==================== CLI ====================

TEST_F(CPUTestBase, CLI) {
    // CLI ; Clear Interrupt Disable
    emulator.cpu().registers().P |= 0x04;  // Set I
    const auto cycles = runProgram({0x58}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_FALSE(emulator.cpu().registers().I());
}

TEST_F(CPUTestBase, CLI_AlreadyClear) {
    // CLI ; Already clear
    emulator.cpu().registers().P &= ~0x04;
    const auto cycles = runProgram({0x58}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(emulator.cpu().registers().I());
}

// ==================== CLV ====================

TEST_F(CPUTestBase, CLV) {
    // CLV ; Clear Overflow
    emulator.cpu().registers().P |= 0x40;  // Set V
    const auto cycles = runProgram({0xB8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_FALSE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, CLV_AlreadyClear) {
    // CLV ; Already clear
    emulator.cpu().registers().P &= ~0x40;
    const auto cycles = runProgram({0xB8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(emulator.cpu().registers().V());
}

// ==================== SEC ====================

TEST_F(CPUTestBase, SEC) {
    // SEC ; Set Carry
    emulator.cpu().registers().P &= ~0x01;  // Clear C
    const auto cycles = runProgram({0x38}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, SEC_AlreadySet) {
    // SEC ; Already set
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x38}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

// ==================== SED ====================

TEST_F(CPUTestBase, SED) {
    // SED ; Set Decimal
    emulator.cpu().registers().P &= ~0x08;  // Clear D
    const auto cycles = runProgram({0xF8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().P & 0x08, 0x08);
}

// ==================== SEI ====================

TEST_F(CPUTestBase, SEI) {
    // SEI ; Set Interrupt Disable
    emulator.cpu().registers().P &= ~0x04;  // Clear I
    const auto cycles = runProgram({0x78}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_TRUE(emulator.cpu().registers().I());
}

TEST_F(CPUTestBase, SEI_AlreadySet) {
    // SEI ; Already set
    emulator.cpu().registers().P |= 0x04;
    const auto cycles = runProgram({0x78}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(emulator.cpu().registers().I());
}

// ==================== Flag Combinations ====================

TEST_F(CPUTestBase, SEC_CLC_RoundTrip) {
    // SEC then CLC
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x38, 0x18}, 2);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, SEI_CLI_RoundTrip) {
    // SEI then CLI
    emulator.cpu().registers().P &= ~0x04;
    const auto cycles = runProgram({0x78, 0x58}, 2);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(emulator.cpu().registers().I());
}
