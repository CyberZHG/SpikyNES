#ifndef SPIKYNES_APU_H
#define SPIKYNES_APU_H

namespace nes_core {

    class Bus;

    class APU {
    public:
        explicit APU(Bus* bus);

        void reset();

    private:
        Bus* bus_ = nullptr;
    };

}

#endif //SPIKYNES_APU_H