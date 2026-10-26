#include "core/mapper/mapper_007.h"

namespace nes_core {
    void Mapper007::reset() {
        Mapper::reset();
        nametableMode_ = 0;
    }

    std::uint8_t Mapper007::cpuRead(const std::uint16_t addr) {
        if (0x8000 <= addr && addr <= 0xFFFF) {
            // 32KB switchable PRG ROM bank
            const auto bankAddr = prgRomIndex_ * 0x8000 + (addr - 0x8000);
            return prgRom_->at(bankAddr % prgRom_->size());
        }
        return 0;
    }

    void Mapper007::cpuWrite(const std::uint16_t addr, std::uint8_t value) {
        if (0x8000 <= addr && addr <= 0xFFFF) {
            if (subMapper_ == 2) {
                // AND-type bus conflicts
                value &= cpuRead(addr);
            }
            prgRomIndex_ = value & 0x7F;
            nametableMode_ = static_cast<bool>(value & 0b0010000);
        }
    }

    std::uint8_t Mapper007::ppuRead(const std::uint16_t addr) {
        if (addr < 0x2000) {
            if (chrRomBanks_ == 0) {
                if (chrRam_ != nullptr && !chrRam_->empty()) {
                    return chrRam_->at(addr);
                }
            } else {
                if (chrRom_ != nullptr && !chrRom_->empty()) {
                    return chrRom_->at(addr);
                }
            }
        }
        return 0;
    }

    void Mapper007::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr < 0x2000) {
            if (chrRomBanks_ == 0) {  // CHR RAM
                if (chrRam_ != nullptr && !chrRam_->empty()) {
                    chrRam_->at(addr) = value;
                }
            }
        }
    }

    Mirroring Mapper007::mirroring() const {
        switch (nametableMode_) {
            case 0: return Mirroring::SINGLE_LOWER;
            case 1: return Mirroring::SINGLE_UPPER;
            default: return Mirroring::INVALID;
        }
    }
}
