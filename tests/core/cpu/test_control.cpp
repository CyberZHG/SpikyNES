#include "test_cpu_utils.h"

// ==================== BRK ====================

TEST_F(CPUTestBase, BRK) {
    // BRK ; Push PC+2, push P with B=1, jump to IRQ vector
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00110100;
    emulator.bus().write(0xFFFE, 0x00);
    emulator.bus().write(0xFFFF, 0x04);
    const auto cycles = runProgram({0x00}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0400);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFC);
    EXPECT_TRUE(emulator.cpu().registers().I());
    EXPECT_EQ(emulator.cpu().read(0x01FF), (CPU_PROGRAM_BEGIN + 2) >> 8);
    EXPECT_EQ(emulator.cpu().read(0x01FE), (CPU_PROGRAM_BEGIN + 2) & 0xFF);
    EXPECT_EQ(emulator.cpu().read(0x01FD) & 0b00110000, 0b00110000);
}

// ==================== RTI ====================

TEST_F(CPUTestBase, RTI) {
    // RTI ; Pull P, pull PC
    emulator.cpu().registers().SP = 0xFC;
    emulator.cpu().registers().P = 0b00110100;
    emulator.cpu().write(0x01FD, 0b11000011);
    emulator.cpu().write(0x01FE, 0x00);
    emulator.cpu().write(0x01FF, 0x03);
    const auto cycles = runProgram({0x40}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0300);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, BRK_RTI_RoundTrip) {
    // BRK then RTI should return to instruction after BRK padding byte
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00110100;
    emulator.bus().write(0xFFFE, 0x10);
    emulator.bus().write(0xFFFF, 0x02);
    emulator.cpu().write(0x0210, 0x40);
    const auto cycles = runProgram({0x00, 0x00, 0xEA}, 3);
    // BRK(7) + RTI(6) + NOP(2) = 15 cycles
    EXPECT_EQ(cycles, 15);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}
