#include "core/controller.h"

namespace nes_core {
    void Controller::reset() {
        strobe_ = false;
        state_ = 0;
        shift_ = 0;
    }

    void Controller::press(Button button) {
        state_ |= (1 << static_cast<int>(button));
    }

    void Controller::release(Button button) {
        state_ &= ~(1 << static_cast<int>(button));
    }

    void Controller::write(const std::uint8_t value) {
        strobe_ = value & 1;
        if (strobe_) {
            shift_ = state_;
        }
    }

    std::uint8_t Controller::read() {
        const std::uint8_t value = shift_ & 1;
        if (!strobe_) {
            shift_ = (shift_ >> 1) | 0x80;
        }
        return 0x40 | value;
    }
}
