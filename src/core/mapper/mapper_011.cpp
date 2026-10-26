#include "core/mapper/mapper_011.h"

namespace nes_core {
    std::uint8_t Mapper011::cpuRead(const std::uint16_t addr) {
        if (0x8000 <= addr && addr <= 0xFFFF) {
            // 32KB switchable PRG ROM bank
            const auto bankAddr = prgRomIndex_ * 0x8000 + (addr - 0x8000);
            return prgRom_->at(bankAddr % prgRom_->size());
        }
        return 0;
    }

    void Mapper011::cpuWrite(const std::uint16_t addr, std::uint8_t value) {
        if (0x8000 <= addr && addr <= 0xFFFF) {
            if (subMapper_ == 2) {
                // AND-type bus conflicts
                value &= cpuRead(addr);
            }
            prgRomIndex_ = value & 0b00000011;
            chrRomIndex_ = (value & 0b11110000) >> 4;
        }
    }

    std::uint8_t Mapper011::ppuRead(const std::uint16_t addr) {
        if (addr < 0x2000) {
            if (chrRomBanks_ == 0) {
                if (chrRam_ != nullptr && !chrRam_->empty()) {
                    return chrRam_->at(addr);
                }
            } else {
                if (chrRom_ != nullptr && !chrRom_->empty()) {
                    // 8KB switchable CHR ROM bank
                    const auto bankAddr = chrRomIndex_ * 0x2000 + addr;
                    return chrRom_->at(bankAddr % chrRom_->size());
                }
            }
        }
        return 0;
    }

    void Mapper011::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr < 0x2000) {
            if (chrRomBanks_ == 0) {  // CHR RAM
                if (chrRam_ != nullptr && !chrRam_->empty()) {
                    chrRam_->at(addr) = value;
                }
            }
        }
    }
}
