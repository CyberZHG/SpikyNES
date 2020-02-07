#include "test_cpu_utils.h"

// ==================== INC ====================

TEST_F(CPUTestBase, INC_ZeroPage) {
    // INC $10
    emulator.cpu().cpuWrite(0x0010, 0x42);
    const auto cycles = runProgram({0xE6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x43);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INC_ZeroPage_Zero) {
    // INC $10 ; 0xFF + 1 = 0x00
    emulator.cpu().cpuWrite(0x0010, 0xFF);
    const auto cycles = runProgram({0xE6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INC_ZeroPage_Negative) {
    // INC $10 ; 0x7F + 1 = 0x80
    emulator.cpu().cpuWrite(0x0010, 0x7F);
    const auto cycles = runProgram({0xE6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INC_ZeroPageX) {
    // INC $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().cpuWrite(0x0015, 0x42);
    const auto cycles = runProgram({0xF6, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0015), 0x43);
}

TEST_F(CPUTestBase, INC_ZeroPageX_Wrap) {
    // INC $F0,X ; Zero page wrap
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().cpuWrite(0x0010, 0x42);
    const auto cycles = runProgram({0xF6, 0xF0}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x43);
}

TEST_F(CPUTestBase, INC_Absolute) {
    // INC $0300
    emulator.cpu().cpuWrite(0x0300, 0x42);
    const auto cycles = runProgram({0xEE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0300), 0x43);
}

TEST_F(CPUTestBase, INC_AbsoluteX) {
    // INC $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().cpuWrite(0x0310, 0x42);
    const auto cycles = runProgram({0xFE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0310), 0x43);
}

// ==================== INX ====================

TEST_F(CPUTestBase, INX) {
    // INX
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0xE8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().X, 0x43);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INX_Zero) {
    // INX ; 0xFF + 1 = 0x00
    emulator.cpu().registers().X = 0xFF;
    const auto cycles = runProgram({0xE8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INX_Negative) {
    // INX ; 0x7F + 1 = 0x80
    emulator.cpu().registers().X = 0x7F;
    const auto cycles = runProgram({0xE8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== INY ====================

TEST_F(CPUTestBase, INY) {
    // INY
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0xC8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x43);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INY_Zero) {
    // INY ; 0xFF + 1 = 0x00
    emulator.cpu().registers().Y = 0xFF;
    const auto cycles = runProgram({0xC8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, INY_Negative) {
    // INY ; 0x7F + 1 = 0x80
    emulator.cpu().registers().Y = 0x7F;
    const auto cycles = runProgram({0xC8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== DEC ====================

TEST_F(CPUTestBase, DEC_ZeroPage) {
    // DEC $10
    emulator.cpu().cpuWrite(0x0010, 0x42);
    const auto cycles = runProgram({0xC6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x41);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEC_ZeroPage_Zero) {
    // DEC $10 ; 0x01 - 1 = 0x00
    emulator.cpu().cpuWrite(0x0010, 0x01);
    const auto cycles = runProgram({0xC6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEC_ZeroPage_Negative) {
    // DEC $10 ; 0x00 - 1 = 0xFF
    emulator.cpu().cpuWrite(0x0010, 0x00);
    const auto cycles = runProgram({0xC6, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEC_ZeroPageX) {
    // DEC $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().cpuWrite(0x0015, 0x42);
    const auto cycles = runProgram({0xD6, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0015), 0x41);
}

TEST_F(CPUTestBase, DEC_ZeroPageX_Wrap) {
    // DEC $F0,X ; Zero page wrap
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().cpuWrite(0x0010, 0x42);
    const auto cycles = runProgram({0xD6, 0xF0}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0010), 0x41);
}

TEST_F(CPUTestBase, DEC_Absolute) {
    // DEC $0300
    emulator.cpu().cpuWrite(0x0300, 0x42);
    const auto cycles = runProgram({0xCE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0300), 0x41);
}

TEST_F(CPUTestBase, DEC_AbsoluteX) {
    // DEC $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().cpuWrite(0x0310, 0x42);
    const auto cycles = runProgram({0xDE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0310), 0x41);
}

// ==================== DEX ====================

TEST_F(CPUTestBase, DEX) {
    // DEX
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0xCA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().X, 0x41);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEX_Zero) {
    // DEX ; 0x01 - 1 = 0x00
    emulator.cpu().registers().X = 0x01;
    const auto cycles = runProgram({0xCA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEX_Negative) {
    // DEX ; 0x00 - 1 = 0xFF
    emulator.cpu().registers().X = 0x00;
    const auto cycles = runProgram({0xCA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== DEY ====================

TEST_F(CPUTestBase, DEY) {
    // DEY
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x88}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x41);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEY_Zero) {
    // DEY ; 0x01 - 1 = 0x00
    emulator.cpu().registers().Y = 0x01;
    const auto cycles = runProgram({0x88}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, DEY_Negative) {
    // DEY ; 0x00 - 1 = 0xFF
    emulator.cpu().registers().Y = 0x00;
    const auto cycles = runProgram({0x88}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}
