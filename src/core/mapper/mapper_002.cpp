#include "core/mapper/mapper_002.h"

namespace nes_core {
    Mapper002::Mapper002() {
        subMapper_ = 2;
    }

    std::uint8_t Mapper002::cpuRead(const std::uint16_t addr) {
        if (0x8000 <= addr && addr <= 0xBFFF) {
            // 16 KB switchable PRG ROM bank
            const auto bankAddr = prgRomIndex_ * PRG_ROM_BANK_SIZE + (addr - 0x8000);
            return prgRom_->at(bankAddr % prgRom_->size());
        }
        if (0xC000 <= addr && addr <= 0xFFFF) {
            // 16 KB PRG ROM bank, fixed to the last bank
            const auto bankAddr = (prgRomBanks_ - 1) * PRG_ROM_BANK_SIZE + (addr - 0xC000);
            return prgRom_->at(bankAddr % prgRom_->size());
        }
        return 0;
    }

    void Mapper002::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x8000 <= addr && addr <= 0xFFFF) {
            const std::uint8_t bankMask = prgRomBanks_ > 0 ? prgRomBanks_ - 1 : 0;
            if (subMapper_ == 2) {
                // AND-type bus conflicts
                prgRomIndex_ = cpuRead(addr) & value & bankMask;
            } else {
                prgRomIndex_ = value & bankMask;
            }
        }
    }

    std::uint8_t Mapper002::ppuRead(const std::uint16_t addr) {
        if (addr >= 0x2000) {
            return 0;
        }
        const auto* chr = (chrRomBanks_ == 0) ? chrRam_ : chrRom_;
        if (chr == nullptr || chr->empty()) {
            return 0;
        }
        return chr->at(addr);
    }

    void Mapper002::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr >= 0x2000) {
            return;
        }
        if (chrRomBanks_ == 0) {
            if (chrRam_ != nullptr && !chrRam_->empty()) {
                chrRam_->at(addr) = value;
            }
        }
    }
}
