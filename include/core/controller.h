#ifndef SPIKYNES_CONTROLLER_H
#define SPIKYNES_CONTROLLER_H
#include <cstdint>

namespace nes_core {
    class Controller {
    public:
        enum class Button {
            A, B, Select, Start,
            Up, Down, Left, Right,
        };

        void reset();

        void press(Button button);
        void release(Button button);

        void write(std::uint8_t value);
        std::uint8_t read();

    private:
        bool strobe_ = false;
        std::uint8_t state_ = 0;
        std::uint8_t shift_ = 0;
    };
}

#endif //SPIKYNES_CONTROLLER_H