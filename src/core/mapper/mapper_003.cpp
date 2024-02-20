#include "core/mapper/mapper_003.h"

namespace nes_core {
    Mapper003::Mapper003() {
        subMapper_ = 2;
    }

    std::uint8_t Mapper003::cpuRead(const std::uint16_t addr) {
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (prgRam_ != nullptr && !prgRam_->empty()) {
                return prgRam_->at(addr - 0x6000);
            }
            return 0;
        }
        if (0x8000 <= addr) {
            auto romAddr = addr - 0x8000;
            if (prgRomBanks_ == 1) {
                romAddr &= 0x3FFF;
            }
            return prgRom_->at(romAddr);
        }
        return 0;
    }

    void Mapper003::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (prgRam_ != nullptr && !prgRam_->empty()) {
                prgRam_->at(addr - 0x6000) = value;
            }
        } else if (0x8000 <= addr && addr <= 0xFFFF) {
            const std::uint8_t bankMask = chrRomBanks_ > 0 ? chrRomBanks_ - 1 : 0;
            if (subMapper_ == 2) {
                // AND-type bus conflicts
                auto romAddr = addr - 0x8000;
                if (prgRomBanks_ == 1) {
                    romAddr &= 0x3FFF;
                }
                chrRomIndex_ = prgRom_->at(romAddr) & value & bankMask;
            } else {
                // No bus conflicts
                chrRomIndex_ = value & bankMask;
            }
        }
    }

    std::uint8_t Mapper003::ppuRead(const std::uint16_t addr) {
        if (addr <= 0x1FFF) {
            if (chrRom_ != nullptr && !chrRom_->empty()) {
                return chrRom_->at(chrRomIndex_ * 0x2000 + addr);
            }
        }
        return 0;
    }

    void Mapper003::ppuWrite([[maybe_unused]] const std::uint16_t addr,
                             [[maybe_unused]] const std::uint8_t value) {
    }
}
