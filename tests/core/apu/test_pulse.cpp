#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class PulseTest : public ::testing::Test {
protected:
    void SetUp() override {
        emulator.reset();
    }

    Emulator emulator;
};

TEST_F(PulseTest, RegisterWrite_Duty) {
    emulator.apu().write(0x4000, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().pulse1.duty, 0b01000000);

    emulator.apu().write(0x4000, 0b01000000);
    EXPECT_EQ(emulator.apu().registers().pulse1.duty, 0b01100000);

    emulator.apu().write(0x4000, 0b10000000);
    EXPECT_EQ(emulator.apu().registers().pulse1.duty, 0b01111000);

    emulator.apu().write(0x4000, 0b11000000);
    EXPECT_EQ(emulator.apu().registers().pulse1.duty, 0b10011111);
}

TEST_F(PulseTest, RegisterWrite_LengthCounterHalt) {
    emulator.apu().write(0x4000, 0b00100000);
    EXPECT_TRUE(emulator.apu().registers().pulse1.lengthCounterHalted);

    emulator.apu().write(0x4000, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().pulse1.lengthCounterHalted);
}

TEST_F(PulseTest, RegisterWrite_ConstantVolume) {
    emulator.apu().write(0x4000, 0b00010000);
    EXPECT_TRUE(emulator.apu().registers().pulse1.constantVolume);

    emulator.apu().write(0x4000, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().pulse1.constantVolume);
}

TEST_F(PulseTest, RegisterWrite_Volume) {
    emulator.apu().write(0x4000, 0b00001111);
    EXPECT_EQ(emulator.apu().registers().pulse1.volume, 15);

    emulator.apu().write(0x4000, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().pulse1.volume, 5);
}

TEST_F(PulseTest, RegisterWrite_SweepUnit) {
    emulator.apu().write(0x4001, 0b10111101);
    EXPECT_TRUE(emulator.apu().registers().pulse1.sweepEnabled);
    EXPECT_EQ(emulator.apu().registers().pulse1.sweepDividerPeriod, 4);
    EXPECT_TRUE(emulator.apu().registers().pulse1.sweepNegate);
    EXPECT_EQ(emulator.apu().registers().pulse1.sweepShift, 5);
    EXPECT_TRUE(emulator.apu().registers().pulse1.sweepReload);
}

TEST_F(PulseTest, RegisterWrite_Timer) {
    emulator.apu().write(0x4002, 0xAB);
    emulator.apu().write(0x4003, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().pulse1.timerPeriod, 0x5AB);
}

TEST_F(PulseTest, RegisterWrite_LengthCounterLoad) {
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 10);

    emulator.apu().write(0x4003, 0b11111000);
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 30);
}

TEST_F(PulseTest, RegisterWrite_EnvelopeRestart) {
    emulator.apu().registers().pulse1.envelopeStart = false;
    emulator.apu().write(0x4003, 0x00);
    EXPECT_TRUE(emulator.apu().registers().pulse1.envelopeStart);
}

TEST_F(PulseTest, RegisterWrite_PhaseReset) {
    emulator.apu().registers().pulse1.dutyBit = 5;
    emulator.apu().write(0x4003, 0x00);
    EXPECT_EQ(emulator.apu().registers().pulse1.dutyBit, 0);
}

TEST_F(PulseTest, Envelope_ConstantVolume) {
    emulator.apu().write(0x4000, 0b00011010);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.dutyBit = 1;
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 10);
}

TEST_F(PulseTest, Envelope_Decay) {
    emulator.apu().write(0x4000, 0b00001111);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().write(0x4015, 0b00000001);

    emulator.apu().registers().pulse1.envelopeStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.envelopeDecayLevel, 15);

    for (int i = 0; i < 16; ++i) {
        emulator.apu().registers().pulse1.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.envelopeDecayLevel, 14);

    for (int i = 0; i < 16 * 14; ++i) {
        emulator.apu().registers().pulse1.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.envelopeDecayLevel, 0);
}

TEST_F(PulseTest, Envelope_Loop) {
    emulator.apu().write(0x4000, 0b00101111);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().write(0x4015, 0b00000001);

    emulator.apu().registers().pulse1.envelopeStep();
    for (int i = 0; i < 16 * 16; ++i) {
        emulator.apu().registers().pulse1.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.envelopeDecayLevel, 15);
}

TEST_F(PulseTest, Envelope_Restart) {
    emulator.apu().write(0x4000, 0b00001010);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().write(0x4015, 0b00000001);

    emulator.apu().registers().pulse1.envelopeStep();
    for (int i = 0; i < 16 * 5; ++i) {
        emulator.apu().registers().pulse1.envelopeStep();
    }
    EXPECT_LT(emulator.apu().registers().pulse1.envelopeDecayLevel, 15);

    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.envelopeStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.envelopeDecayLevel, 15);
}

TEST_F(PulseTest, LengthCounter_Decrement) {
    emulator.apu().write(0x4000, 0b00000000);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 10);

    emulator.apu().registers().pulse1.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 9);

    for (int i = 0; i < 9; ++i) {
        emulator.apu().registers().pulse1.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 0);

    emulator.apu().registers().pulse1.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 0);
}

TEST_F(PulseTest, LengthCounter_Halt) {
    emulator.apu().write(0x4000, 0b00100000);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 10);

    for (int i = 0; i < 20; ++i) {
        emulator.apu().registers().pulse1.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.lengthCounter, 10);
}

TEST_F(PulseTest, LengthCounter_Mutes) {
    emulator.apu().write(0x4000, 0b00011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.dutyBit = 1;

    EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);

    for (int i = 0; i < 10; ++i) {
        emulator.apu().registers().pulse1.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
}

TEST_F(PulseTest, Sweep_Pulse1_Negate) {
    emulator.apu().write(0x4000, 0b00000000);
    emulator.apu().write(0x4001, 0b10001001);
    emulator.apu().write(0x4002, 0x00);
    emulator.apu().write(0x4003, 0b00010010);

    EXPECT_EQ(emulator.apu().registers().pulse1.timerPeriod, 0x200);

    emulator.apu().registers().pulse1.sweepStep(true);
    EXPECT_EQ(emulator.apu().registers().pulse1.timerPeriod, 0x200 - (0x200 >> 1) - 1);
}

TEST_F(PulseTest, Sweep_Pulse2_Negate) {
    emulator.apu().write(0x4004, 0b00000000);
    emulator.apu().write(0x4005, 0b10001001);
    emulator.apu().write(0x4006, 0x00);
    emulator.apu().write(0x4007, 0b00010010);

    EXPECT_EQ(emulator.apu().registers().pulse2.timerPeriod, 0x200);

    emulator.apu().registers().pulse2.sweepStep(false);
    EXPECT_EQ(emulator.apu().registers().pulse2.timerPeriod, 0x200 - (0x200 >> 1));
}

TEST_F(PulseTest, Sweep_ReloadFlag) {
    emulator.apu().write(0x4001, 0b00010001);
    EXPECT_TRUE(emulator.apu().registers().pulse1.sweepReload);

    emulator.apu().registers().pulse1.sweepStep(true);
    EXPECT_FALSE(emulator.apu().registers().pulse1.sweepReload);
}

TEST_F(PulseTest, Sweep_DividerReload) {
    emulator.apu().write(0x4001, 0b00110001);
    emulator.apu().registers().pulse1.sweepCounter = 5;

    emulator.apu().registers().pulse1.sweepStep(true);
    EXPECT_EQ(emulator.apu().registers().pulse1.sweepCounter, 4);
}

TEST_F(PulseTest, Timer_Countdown) {
    emulator.apu().write(0x4002, 0x05);
    emulator.apu().write(0x4003, 0x00);

    emulator.apu().registers().pulse1.timer = 5;
    emulator.apu().registers().pulse1.timerStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.timer, 4);

    for (int i = 0; i < 4; ++i) {
        emulator.apu().registers().pulse1.timerStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.timer, 0);

    emulator.apu().registers().pulse1.timerStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.timer, 5);
}

TEST_F(PulseTest, Timer_SequencerAdvance) {
    emulator.apu().write(0x4002, 0x00);
    emulator.apu().write(0x4003, 0x00);

    emulator.apu().registers().pulse1.dutyBit = 0;
    emulator.apu().registers().pulse1.timer = 0;

    emulator.apu().registers().pulse1.timerStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.dutyBit, 1);

    for (int i = 0; i < 7; ++i) {
        emulator.apu().registers().pulse1.timerStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.dutyBit, 0);
}

TEST_F(PulseTest, Output_TimerTooLow) {
    emulator.apu().write(0x4000, 0b00011111);
    emulator.apu().write(0x4002, 0x07);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.dutyBit = 1;

    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);

    emulator.apu().write(0x4002, 0x08);
    EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);
}

TEST_F(PulseTest, Output_TimerTooHigh) {
    emulator.apu().write(0x4000, 0b00011111);
    emulator.apu().write(0x4002, 0xFF);
    emulator.apu().write(0x4003, 0b00000111);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().registers().pulse1.dutyBit = 1;

    emulator.apu().registers().pulse1.timerPeriod = 0x800;
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
}

TEST_F(PulseTest, Output_DutyCycle_12_5) {
    emulator.apu().write(0x4000, 0b00011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    emulator.apu().registers().pulse1.dutyBit = 0;
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);

    emulator.apu().registers().pulse1.dutyBit = 1;
    EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);

    for (int i = 2; i < 8; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
    }
}

TEST_F(PulseTest, Output_DutyCycle_25) {
    emulator.apu().write(0x4000, 0b01011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    emulator.apu().registers().pulse1.dutyBit = 0;
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);

    for (int i = 1; i < 3; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);
    }

    for (int i = 3; i < 8; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
    }
}

TEST_F(PulseTest, Output_DutyCycle_50) {
    emulator.apu().write(0x4000, 0b10011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    emulator.apu().registers().pulse1.dutyBit = 0;
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);

    for (int i = 1; i < 5; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);
    }

    for (int i = 5; i < 8; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
    }
}

TEST_F(PulseTest, Output_DutyCycle_75) {
    emulator.apu().write(0x4000, 0b11011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);

    emulator.apu().registers().pulse1.dutyBit = 0;
    EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);

    for (int i = 1; i < 3; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_EQ(emulator.apu().registers().pulse1.output(), 0);
    }

    for (int i = 3; i < 8; ++i) {
        emulator.apu().registers().pulse1.dutyBit = i;
        EXPECT_GT(emulator.apu().registers().pulse1.output(), 0);
    }
}

TEST_F(PulseTest, Output_VolumeConstant) {
    emulator.apu().write(0x4000, 0b00011010);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.dutyBit = 1;

    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 10);
}

TEST_F(PulseTest, Output_VolumeEnvelope) {
    emulator.apu().write(0x4000, 0b00001111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().registers().pulse1.dutyBit = 1;

    emulator.apu().registers().pulse1.envelopeStep();
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 15);

    for (int i = 0; i < 16; ++i) {
        emulator.apu().registers().pulse1.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().pulse1.output(), 14);
}

TEST_F(PulseTest, Enable_DisablesOutput) {
    emulator.apu().write(0x4000, 0b11011111);
    emulator.apu().write(0x4002, 0x10);
    emulator.apu().write(0x4015, 0b00000001);
    emulator.apu().write(0x4003, 0b00000000);
    emulator.apu().step();
    EXPECT_GT(emulator.apu().outputBuffer(), 0.0f);

    emulator.apu().write(0x4015, 0b00000000);
    emulator.apu().step();
    EXPECT_EQ(emulator.apu().outputBuffer(), 0.0f);
}

TEST_F(PulseTest, Enable_Pulse2) {
    emulator.apu().write(0x4004, 0b11011111);
    emulator.apu().write(0x4006, 0x10);
    emulator.apu().write(0x4015, 0b00000010);
    emulator.apu().write(0x4007, 0b00000000);
    emulator.apu().step();
    EXPECT_GT(emulator.apu().outputBuffer(), 0.0f);

    emulator.apu().write(0x4015, 0b00000000);
    emulator.apu().step();
    EXPECT_EQ(emulator.apu().outputBuffer(), 0.0f);
}
