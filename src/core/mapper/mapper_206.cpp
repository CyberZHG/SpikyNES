#include "core/mapper/mapper_206.h"

namespace nes_core {
    Mapper206::Mapper206() {
    }

    void Mapper206::reset() {
        Mapper::reset();
        bankToUpdate_ = 0;
        romIndices_.fill(0);
    }

    std::uint8_t Mapper206::cpuRead(const std::uint16_t addr) {
        if (addr <= 0x7FFF) {
            return 0;
        }
        int bankAddr = 0;
        if (0x8000 <= addr && addr <= 0x9FFF) {
            bankAddr = romIndices_[6] * 0x2000 + (addr - 0x8000);
        } else if (0xA000 <= addr && addr <= 0xBFFF) {
            bankAddr = romIndices_[7] * 0x2000 + (addr - 0xA000);
        } else if (0xC000 <= addr && addr <= 0xDFFF) {
            bankAddr = (prgRomBanks_ * 2 - 2) * 0x2000 + (addr - 0xC000);
        } else {
            bankAddr = (prgRomBanks_ * 2 - 1) * 0x2000 + (addr - 0xE000);
        }
        return prgRom_->at(bankAddr % prgRom_->size());
    }

    void Mapper206::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x8000 <= addr && addr <= 0x9FFF) {
            if (addr % 2 == 0) {
                bankToUpdate_ = value & 0b00000111;
            } else {
                romIndices_[bankToUpdate_] = value;
            }
        }
    }

    int Mapper206::chrBankAddr(const std::uint16_t addr) const {
        int bankAddr = 0;
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
        return bankAddr;
    }

    std::uint8_t Mapper206::ppuRead(const std::uint16_t addr) {
        if (addr >= 0x2000) {
            return 0;
        }
        const auto* chr = (chrRomBanks_ == 0) ? chrRam_ : chrRom_;
        if (chr == nullptr || chr->empty()) {
            return 0;
        }
        return chr->at(chrBankAddr(addr) % chr->size());
    }

    void Mapper206::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr >= 0x2000) {
            return;
        }
        if (chrRomBanks_ == 0 && chrRam_ != nullptr && !chrRam_->empty()) {
            chrRam_->at(chrBankAddr(addr) % chrRam_->size()) = value;
        }
    }
}