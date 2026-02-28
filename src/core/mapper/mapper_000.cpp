#include "core/mapper/mapper_000.h"

namespace nes_core {
    std::uint8_t Mapper000::cpuRead(const std::uint16_t addr) {
        if (0x6000 <= addr && addr < 0x8000) {
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

    void Mapper000::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x6000 <= addr && addr < 0x8000) {
            if (prgRam_ != nullptr && !prgRam_->empty()) {
                prgRam_->at(addr - 0x6000) = value;
            }
        }
    }

    std::uint8_t Mapper000::ppuRead(const std::uint16_t addr) {
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

    void Mapper000::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (addr < 0x2000) {
            if (chrRomBanks_ == 0) {  // CHR RAM
                if (chrRam_ != nullptr && !chrRam_->empty()) {
                    chrRam_->at(addr) = value;
                }
            }
        }
    }
}
