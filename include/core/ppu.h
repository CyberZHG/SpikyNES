#ifndef SPIKYNES_PPU_H
#define SPIKYNES_PPU_H

namespace nes_core {

    class Bus;

    class PPU {
    public:
        explicit PPU(Bus* bus);

        void reset();

    private:
        Bus* bus_ = nullptr;
    };

}

#endif //SPIKYNES_PPU_H