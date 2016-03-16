#include "core/mapper/mapper_004.h"

#include "core/bus.h"

namespace nes_core {
    Mapper004::Mapper004() {
    }

    void Mapper004::reset() {
        Mapper::reset();
        bankToUpdate_ = 0;
        prgRomBankMode_ = 0;
        chrRomBankMode_ = 0;
        nametableMode_ = 0;
        romIndices_.fill(0);
        writeProtected_ = false;
        prgRamEnabled_ = false;
        irqLatch_ = 0;
        irqCounter_ = 0;
        irqDisabled_ = false;
        irqReload_ = false;
        irqFlag_ = false;
        a12LowCycle_ = 0;
        irqDelay_ = 0;
    }

    std::uint8_t Mapper004::cpuRead(const std::uint16_t addr) {
        if (addr <= 0x5FFF) {
            return 0;
        }
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (!prgRamEnabled_) {
                return bus_->lastBusValue();
            }
            if (prgRam_ != nullptr && !prgRam_->empty()) {
                return prgRam_->at(addr - 0x6000);
            }
        }
        int bankAddr = 0;
        if (prgRomBankMode_ == 0) {
            if (0x8000 <= addr && addr <= 0x9FFF) {
                bankAddr = romIndices_[6] * 0x2000 + (addr - 0x8000);
            } else if (0xA000 <= addr && addr <= 0xBFFF) {
                bankAddr = romIndices_[7] * 0x2000 + (addr - 0xA000);
            } else if (0xC000 <= addr && addr <= 0xDFFF) {
                bankAddr = (prgRomBanks_ * 2 - 2) * 0x2000 + (addr - 0xC000);
            } else {
                bankAddr = (prgRomBanks_ * 2 - 1) * 0x2000 + (addr - 0xE000);
            }
        } else {
            if (0x8000 <= addr && addr <= 0x9FFF) {
                bankAddr = (prgRomBanks_ * 2 - 2) * 0x2000 + (addr - 0x8000);
            } else if (0xA000 <= addr && addr <= 0xBFFF) {
                bankAddr = romIndices_[7] * 0x2000 + (addr - 0xA000);
            } else if (0xC000 <= addr && addr <= 0xDFFF) {
                bankAddr = romIndices_[6] * 0x2000 + (addr - 0xC000);
            } else {
                bankAddr = (prgRomBanks_ * 2 - 1) * 0x2000 + (addr - 0xE000);
            }
        }
        return prgRom_->at(bankAddr % prgRom_->size());
    }

    void Mapper004::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (!prgRamEnabled_) {
                return;
            }
            if (!writeProtected_ && prgRam_ != nullptr && !prgRam_->empty()) {
                prgRam_->at(addr - 0x6000) = value;
            }
        } else if (0x8000 <= addr && addr <= 0x9FFF) {
            if (addr % 2 == 0) {
                bankToUpdate_ = value & 0b00000111;
                prgRomBankMode_ = (value >> 6) & 1;
                chrRomBankMode_ = (value >> 7) & 1;
            } else {
                romIndices_[bankToUpdate_] = value;
            }
        } else if (0xA000 <= addr && addr <= 0xBFFF) {
            if (addr % 2 == 0) {
                nametableMode_ = value & 1;
            } else {
                writeProtected_ = (value >> 6) & 1;
                prgRamEnabled_ = (value >> 7) & 1;
            }
        } else if (0xC000 <= addr && addr <= 0xDFFF) {
            if (addr % 2 == 0) {
                irqLatch_ = value;
            } else {
                irqReload_ = true;
                irqCounter_ = 0;
            }
        } else if (0xE000 <= addr && addr <= 0xFFFF) {
            if (addr % 2 == 0) {
                irqDisabled_ = true;
                irqFlag_ = false;
                irqDelay_ = 0;
            } else {
                irqDisabled_ = false;
            }
        }
    }

    int Mapper004::chrBankAddr(const std::uint16_t addr) const {
        int bankAddr = 0;
        if (chrRomBankMode_ == 0) {
            if (addr <= 0x07FF) {
                bankAddr = (romIndices_[0] & 0b11111110) * 0x0400 + addr;
            } else if (addr <= 0x0FFF) {
                bankAddr = (romIndices_[1] & 0b11111110) * 0x0400 + (addr - 0x0800);
            } else if (addr <= 0x13FF) {
                bankAddr = romIndices_[2] * 0x0400 + (addr - 0x1000);
            } else if (addr <= 0x17FF) {
                bankAddr = romIndices_[3] * 0x0400 + (addr - 0x1400);
            } else if (addr <= 0x1BFF) {
                bankAddr = romIndices_[4] * 0x0400 + (addr - 0x1800);
            } else {
                bankAddr = romIndices_[5] * 0x0400 + (addr - 0x1C00);
            }
        } else {
            if (addr <= 0x03FF) {
                bankAddr = romIndices_[2] * 0x0400 + addr;
            } else if (addr <= 0x07FF) {
                bankAddr = romIndices_[3] * 0x0400 + (addr - 0x0400);
            } else if (addr <= 0x0BFF) {
                bankAddr = romIndices_[4] * 0x0400 + (addr - 0x0800);
            } else if (addr <= 0x0FFF) {
                bankAddr = romIndices_[5] * 0x0400 + (addr - 0x0C00);
            } else if (addr <= 0x17FF) {
                bankAddr = (romIndices_[0] & 0b11111110) * 0x0400 + (addr - 0x1000);
            } else {
                bankAddr = (romIndices_[1] & 0b11111110) * 0x0400 + (addr - 0x1800);
            }
        }
        return bankAddr;
    }

    std::uint8_t Mapper004::ppuRead(const std::uint16_t addr) {
        if (addr >= 0x2000) {
            return 0;
        }
        const auto* chr = (chrRomBanks_ == 0) ? chrRam_ : chrRom_;
        if (chr == nullptr || chr->empty()) {
            return 0;
        }
        return chr->at(chrBankAddr(addr) % chr->size());
    }

    void Mapper004::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr >= 0x2000) {
            return;
        }
        if (chrRomBanks_ == 0 && chrRam_ != nullptr && !chrRam_->empty()) {
            chrRam_->at(chrBankAddr(addr) % chrRam_->size()) = value;
        }
    }

    Mirroring Mapper004::mirroring() const {
        return nametableMode_ == 0 ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
    }

    bool Mapper004::irqAsserted() const {
        return irqFlag_;
    }

    void Mapper004::notifyA12(const std::uint16_t addr) {
        if (bus_ == nullptr) {
            return;
        }
        if (addr & 0x1000) {
            if (a12LowCycle_ > 0) {
                if (bus_->ppuCycles() >= a12LowCycle_ + 10) {
                    clockIrqCounter();
                }
                a12LowCycle_ = 0;
            }
        } else if (a12LowCycle_ == 0) {
            a12LowCycle_ = bus_->ppuCycles();
        }
    }

    void Mapper004::clockIrqCounter() {
        if (irqCounter_ == 0 || irqReload_) {
            irqCounter_ = irqLatch_;
        } else {
            --irqCounter_;
        }
        irqReload_ = false;
        if (irqCounter_ == 0 && !irqDisabled_) {
            irqFlag_ = true;
        }
    }

    void Mapper004::ppuStep() {
    }

}
