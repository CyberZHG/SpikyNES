#include <gtest/gtest.h>
#include "core/controller.h"

using namespace nes_core;

class ControllerTest : public testing::Test {
protected:
    Controller controller;

    void SetUp() override {
        controller.reset();
    }

    void strobeAndRead() {
        controller.write(1);
        controller.write(0);
    }
};

TEST_F(ControllerTest, InitialState) {
    strobeAndRead();
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(controller.read(), 0x40);
    }
}

TEST_F(ControllerTest, SingleButtonPress) {
    controller.press(Controller::Button::A);
    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x41);
    for (int i = 1; i < 8; ++i) {
        EXPECT_EQ(controller.read(), 0x40);
    }
}

TEST_F(ControllerTest, AllButtonsPressed) {
    controller.press(Controller::Button::A);
    controller.press(Controller::Button::B);
    controller.press(Controller::Button::Select);
    controller.press(Controller::Button::Start);
    controller.press(Controller::Button::Up);
    controller.press(Controller::Button::Down);
    controller.press(Controller::Button::Left);
    controller.press(Controller::Button::Right);

    strobeAndRead();
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(controller.read(), 0x41);
    }
}

TEST_F(ControllerTest, ButtonOrder) {
    controller.press(Controller::Button::A);
    controller.press(Controller::Button::Select);
    controller.press(Controller::Button::Up);
    controller.press(Controller::Button::Left);

    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x41);  // A
    EXPECT_EQ(controller.read(), 0x40);  // B
    EXPECT_EQ(controller.read(), 0x41);  // Select
    EXPECT_EQ(controller.read(), 0x40);  // Start
    EXPECT_EQ(controller.read(), 0x41);  // Up
    EXPECT_EQ(controller.read(), 0x40);  // Down
    EXPECT_EQ(controller.read(), 0x41);  // Left
    EXPECT_EQ(controller.read(), 0x40);  // Right
}

TEST_F(ControllerTest, SubsequentReadsReturnOne) {
    strobeAndRead();
    for (int i = 0; i < 8; ++i) {
        controller.read();
    }
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(controller.read(), 0x41);
    }
}

TEST_F(ControllerTest, ButtonRelease) {
    controller.press(Controller::Button::A);
    controller.press(Controller::Button::B);

    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x41);  // A pressed
    EXPECT_EQ(controller.read(), 0x41);  // B pressed

    controller.release(Controller::Button::A);
    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x40);  // A released
    EXPECT_EQ(controller.read(), 0x41);  // B still pressed
}

TEST_F(ControllerTest, StrobeHighKeepsReturningFirstButton) {
    controller.press(Controller::Button::A);
    controller.write(1);

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(controller.read(), 0x41);
    }

    controller.write(0);
    EXPECT_EQ(controller.read(), 0x41);  // A
    EXPECT_EQ(controller.read(), 0x40);  // B
}

TEST_F(ControllerTest, MultipleStrobeSequences) {
    controller.press(Controller::Button::Start);

    strobeAndRead();
    controller.read();  // A
    controller.read();  // B
    controller.read();  // Select
    EXPECT_EQ(controller.read(), 0x41);  // Start

    strobeAndRead();
    controller.read();  // A
    controller.read();  // B
    controller.read();  // Select
    EXPECT_EQ(controller.read(), 0x41);  // Start again
}

TEST_F(ControllerTest, ChangeButtonsDuringRead) {
    controller.press(Controller::Button::A);
    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x41);  // A

    controller.press(Controller::Button::B);
    EXPECT_EQ(controller.read(), 0x40);  // B not updated yet

    strobeAndRead();
    EXPECT_EQ(controller.read(), 0x41);  // A
    EXPECT_EQ(controller.read(), 0x41);  // B now updated
}

TEST_F(ControllerTest, DirectionalButtons) {
    controller.press(Controller::Button::Up);
    controller.press(Controller::Button::Right);

    strobeAndRead();
    for (int i = 0; i < 4; ++i) {
        controller.read();
    }
    EXPECT_EQ(controller.read(), 0x41);  // Up
    EXPECT_EQ(controller.read(), 0x40);  // Down
    EXPECT_EQ(controller.read(), 0x40);  // Left
    EXPECT_EQ(controller.read(), 0x41);  // Right
}

TEST_F(ControllerTest, ResetClearsState) {
    controller.press(Controller::Button::A);
    controller.press(Controller::Button::B);
    controller.reset();

    strobeAndRead();
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(controller.read(), 0x40);
    }
}
