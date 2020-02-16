#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class TriangleTest : public ::testing::Test {
protected:
    void SetUp() override {
        emulator.reset();
    }

    Emulator emulator;
};

TEST_F(TriangleTest, RegisterWrite_LengthCounterHalt) {
    emulator.apu().write(0x4008, 0b10000000);
    EXPECT_TRUE(emulator.apu().registers().triangle.lengthCounterHalted);

    emulator.apu().write(0x4008, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().triangle.lengthCounterHalted);
}

TEST_F(TriangleTest, RegisterWrite_LinearCounterReload) {
    emulator.apu().write(0x4008, 0b01111111);
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounterReload, 127);

    emulator.apu().write(0x4008, 0b10101010);
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounterReload, 0b0101010);
}

TEST_F(TriangleTest, RegisterWrite_Timer) {
    emulator.apu().write(0x400A, 0xAB);
    emulator.apu().write(0x400B, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().triangle.timerPeriod, 0x5AB);
}

TEST_F(TriangleTest, RegisterWrite_LengthCounterLoad) {
    emulator.apu().write(0x400B, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 10);

    emulator.apu().write(0x400B, 0b11111000);
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 30);
}

TEST_F(TriangleTest, RegisterWrite_LinearStartOnWrite400B) {
    emulator.apu().registers().triangle.linearStart = false;
    emulator.apu().write(0x400B, 0x00);
    EXPECT_TRUE(emulator.apu().registers().triangle.linearStart);
}

TEST_F(TriangleTest, LinearCounter_ReloadOnLinearStart) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0x00);
    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounter, 15);
}

TEST_F(TriangleTest, LinearCounter_Decrement) {
    emulator.apu().write(0x4008, 0b00000001);
    emulator.apu().write(0x400B, 0x00);
    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounter, 1);

    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounter, 0);

    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.linearCounter, 0);
}

TEST_F(TriangleTest, LinearCounter_HaltKeepsLinearStart) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0x00);
    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_TRUE(emulator.apu().registers().triangle.linearStart);

    emulator.apu().write(0x4008, 0b00001111);
    emulator.apu().registers().triangle.linearStart = true;
    emulator.apu().registers().triangle.linearCounterStep();
    EXPECT_FALSE(emulator.apu().registers().triangle.linearStart);
}

TEST_F(TriangleTest, LengthCounter_Decrement) {
    emulator.apu().write(0x4008, 0b00000000);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);

    emulator.apu().registers().triangle.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 9);

    for (int i = 0; i < 9; ++i) {
        emulator.apu().registers().triangle.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 0);

    emulator.apu().registers().triangle.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 0);
}

TEST_F(TriangleTest, LengthCounter_Halt) {
    emulator.apu().write(0x4008, 0b10000000);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);

    for (int i = 0; i < 20; ++i) {
        emulator.apu().registers().triangle.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 10);
}

TEST_F(TriangleTest, Timer_Countdown) {
    emulator.apu().write(0x400A, 0x05);
    emulator.apu().write(0x400B, 0x00);

    emulator.apu().registers().triangle.timer = 5;
    emulator.apu().registers().triangle.timerStep();
    EXPECT_EQ(emulator.apu().registers().triangle.timer, 4);

    for (int i = 0; i < 4; ++i) {
        emulator.apu().registers().triangle.timerStep();
    }
    EXPECT_EQ(emulator.apu().registers().triangle.timer, 0);

    emulator.apu().registers().triangle.timerStep();
    EXPECT_EQ(emulator.apu().registers().triangle.timer, 5);
}

TEST_F(TriangleTest, Timer_SequencerAdvance) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400A, 0x00);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().registers().triangle.linearCounterStep();

    emulator.apu().registers().triangle.dutyIndex = 0;
    emulator.apu().registers().triangle.timer = 0;
    emulator.apu().registers().triangle.timerStep();
    EXPECT_EQ(emulator.apu().registers().triangle.dutyIndex, 1);

    for (int i = 0; i < 31; ++i) {
        emulator.apu().registers().triangle.timerStep();
    }
    EXPECT_EQ(emulator.apu().registers().triangle.dutyIndex, 0);
}

TEST_F(TriangleTest, Timer_SilencedWhenLengthCounterZero) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().registers().triangle.linearCounterStep();

    emulator.apu().registers().triangle.timer = 0;
    emulator.apu().registers().triangle.dutyIndex = 5;
    emulator.apu().registers().triangle.lengthCounter = 0;
    emulator.apu().registers().triangle.timerStep();
    EXPECT_EQ(emulator.apu().registers().triangle.dutyIndex, 5);
}

TEST_F(TriangleTest, Timer_SilencedWhenLinearCounterZero) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);

    emulator.apu().registers().triangle.timer = 0;
    emulator.apu().registers().triangle.dutyIndex = 5;
    emulator.apu().registers().triangle.linearCounter = 0;
    emulator.apu().registers().triangle.timerStep();
    EXPECT_EQ(emulator.apu().registers().triangle.dutyIndex, 5);
}

TEST_F(TriangleTest, Output_SequencerValues) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().registers().triangle.linearCounterStep();

    for (int i = 0; i < 16; ++i) {
        emulator.apu().registers().triangle.dutyIndex = i;
        EXPECT_EQ(emulator.apu().registers().triangle.output(), 15 - i);
    }
    for (int i = 0; i < 16; ++i) {
        emulator.apu().registers().triangle.dutyIndex = 16 + i;
        EXPECT_EQ(emulator.apu().registers().triangle.output(), i);
    }
}

TEST_F(TriangleTest, Output_SilencedByLengthCounter) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().registers().triangle.linearCounterStep();

    emulator.apu().registers().triangle.dutyIndex = 0;
    EXPECT_GT(emulator.apu().registers().triangle.output(), 0);

    emulator.apu().registers().triangle.lengthCounter = 0;
    EXPECT_EQ(emulator.apu().registers().triangle.output(), 0);
}

TEST_F(TriangleTest, Output_SilencedByLinearCounter) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().registers().triangle.linearCounterStep();

    emulator.apu().registers().triangle.dutyIndex = 0;
    EXPECT_GT(emulator.apu().registers().triangle.output(), 0);

    emulator.apu().registers().triangle.linearCounter = 0;
    EXPECT_EQ(emulator.apu().registers().triangle.output(), 0);
}

TEST_F(TriangleTest, Enable_ProducesOutput) {
    emulator.apu().write(0x4008, 0b10001111);
    emulator.apu().write(0x400A, 0x10);
    emulator.apu().write(0x400B, 0b00000000);
    emulator.apu().registers().triangle.linearCounter = 15;

    emulator.apu().write(0x4015, 0b00000100);
    emulator.apu().step();
    EXPECT_GT(emulator.apu().outputBuffer(), 0.0f);

    emulator.apu().write(0x4015, 0b00000000);
    emulator.apu().step();
    EXPECT_EQ(emulator.apu().outputBuffer(), 0.0f);
}

TEST_F(TriangleTest, Enable_ClearsLengthCounter) {
    emulator.apu().write(0x400B, 0b11111000);
    emulator.apu().write(0x4015, 0b00000100);
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 30);

    emulator.apu().write(0x4015, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().triangle.lengthCounter, 0);
}
