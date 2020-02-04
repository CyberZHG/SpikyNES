#include "test_cpu_utils.h"

// ==================== STA ====================

TEST_F(CPUTestBase, STA_Absolute) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STA_Absolute_NoFlagChange) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x00);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, STA_Absolute_NoFlagChange_Negative) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x80;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, STA_AbsoluteX) {
    // STA $0300,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x10;
    const auto cycles = runProgram({0x9D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x42);
}

TEST_F(CPUTestBase, STA_AbsoluteY) {
    // STA $0300,Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x10;
    const auto cycles = runProgram({0x99, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPage) {
    // STA $10
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0x85, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPageX) {
    // STA $10,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x95, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPageX_Wrap) {
    // STA $F0,X ; Zero page wrap
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x20;
    const auto cycles = runProgram({0x95, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STA_IndirectX) {
    // STA ($10,X)
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    const auto cycles = runProgram({0x81, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STA_IndirectY) {
    // STA ($10),Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    const auto cycles = runProgram({0x91, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x42);
}

// ==================== STX ====================

TEST_F(CPUTestBase, STX_Absolute) {
    // STX $0300
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0x8E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPage) {
    // STX $10
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0x86, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPageY) {
    // STX $10,Y
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x96, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPageY_Wrap) {
    // STX $F0,Y ; Zero page wrap
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().registers().Y = 0x20;
    const auto cycles = runProgram({0x96, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

// ==================== STY ====================

TEST_F(CPUTestBase, STY_Absolute) {
    // STY $0300
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x8C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPage) {
    // STY $10
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x84, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPageX) {
    // STY $10,X
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x94, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPageX_Wrap) {
    // STY $F0,X ; Zero page wrap
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().registers().X = 0x20;
    const auto cycles = runProgram({0x94, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}
