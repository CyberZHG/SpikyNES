#include "core/mapper/mapper_001.h"

#include "core/bus.h"
#include "core/constants.h"

static constexpr std::size_t CHR_BANK_4K = 0x1000;

namespace nes_core {
    Mapper001::Mapper001() {
        subMapper_ = 2;
    }

    std::uint8_t Mapper001::cpuRead(const std::uint16_t addr) {
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (!prgRamDisabled_ && prgRam_ != nullptr && !prgRam_->empty()) {
                return prgRam_->at(addr - 0x6000);
            }
        } else if (0x8000 <= addr && addr <= 0xFFFF) {
            if (prgRomBankMode_ == 0 || prgRomBankMode_ == 1) {
                // switch 32 KB at $8000, ignoring low bit of bank number
                const auto bankAddr = (prgRomIndex_ & 0b11110) * PRG_ROM_BANK_SIZE + (addr - 0x8000);
                return prgRom_->at(bankAddr % prgRom_->size());
            }
            if (prgRomBankMode_ == 2) {
                // fix first bank at $8000 and switch 16 KB bank at $C000
                if (0x8000 <= addr && addr <= 0xBFFF) {
                    return prgRom_->at(addr - 0x8000);
                }
                const auto bankAddr = prgRomIndex_ * PRG_ROM_BANK_SIZE + (addr - 0xC000);
                return prgRom_->at(bankAddr % prgRom_->size());
            }
            if (prgRomBankMode_ == 3) {
                // fix last bank at $C000 and switch 16 KB bank at $8000
                if (0x8000 <= addr && addr <= 0xBFFF) {
                    const auto bankAddr = prgRomIndex_ * PRG_ROM_BANK_SIZE + (addr - 0x8000);
                    return prgRom_->at(bankAddr % prgRom_->size());
                }
                const auto bankAddr = (prgRomBanks_ - 1) * PRG_ROM_BANK_SIZE + (addr - 0xC000);
                return prgRom_->at(bankAddr % prgRom_->size());
            }
        }
        return 0;
    }

    void Mapper001::cpuWrite(const std::uint16_t addr, const std::uint8_t value) {
        if (0x6000 <= addr && addr <= 0x7FFF) {
            if (!prgRamDisabled_ && prgRam_ != nullptr && !prgRam_->empty()) {
                prgRam_->at(addr - 0x6000) = value;
            }
        } else if (0x8000 <= addr && addr <= 0xFFFF) {
            if (value & 0b10000000) {
                shiftRegister_ = 0b10000;
                prgRomBankMode_ = 3;
                lastWriteCycle_ = bus_ ? bus_->cpuCycles() : lastWriteCycle_;
                return;
            }
            const std::int64_t cycle = bus_ ? bus_->cpuCycles() : (lastWriteCycle_ + 2);
            const bool isConsecutive = (cycle == lastWriteCycle_ || cycle == lastWriteCycle_ + 1);
            lastWriteCycle_ = cycle;
            if (isConsecutive) {
                return;
            }
            const bool isFull = shiftRegister_ & 1;
            shiftRegister_ >>= 1;
            shiftRegister_ |= (value & 1) << 4;
            if (isFull) {
                writeRegister(addr, shiftRegister_);
                shiftRegister_ = 0b10000;
            }
        }
    }

    std::uint8_t Mapper001::ppuRead(const std::uint16_t addr) {
        if (addr > 0x1FFF) {
            return 0;
        }
        if (chrRomBankMode_ == 0) {
            // switch 8 KB at a time; low bit of index ignored
            const auto bankAddr = (chrRomIndex0_ & 0b11110) * CHR_BANK_4K + addr;
            return chrRom_->at(bankAddr);
        }
        // switch two separate 4 KB banks
        if (addr <= 0x0FFF) {
            const auto bankAddr = (chrRomIndex0_ & 0b11111) * CHR_BANK_4K + addr;
            return chrRom_->at(bankAddr);
        }
        const auto bankAddr = (chrRomIndex1_ & 0b11111) * CHR_BANK_4K + (addr - 0x1000);
        return chrRom_->at(bankAddr);
    }

    void Mapper001::ppuWrite(const std::uint16_t addr, const std::uint8_t value) {
        // CHR-ROM is read-only; writes only apply when using CHR-RAM (chrRomBanks == 0)
        if (chrRomBanks_ != 0 || chrRom_ == nullptr || chrRom_->empty()) {
            return;
        }
        if (addr > 0x1FFF) {
            return;
        }
        std::size_t bankAddr;
        if (chrRomBankMode_ == 0) {
            bankAddr = (chrRomIndex0_ & 0b11110) * CHR_BANK_4K + addr;
        } else if (addr <= 0x0FFF) {
            bankAddr = (chrRomIndex0_ & 0b11111) * CHR_BANK_4K + addr;
        } else {
            bankAddr = (chrRomIndex1_ & 0b11111) * CHR_BANK_4K + (addr - 0x1000);
        }
        if (bankAddr < chrRom_->size()) {
            chrRom_->at(bankAddr) = value;
        }
    }

    Mirroring Mapper001::mirroring() const {
        switch (nametableMode_) {
            case 0: return Mirroring::SINGLE_LOWER;
            case 1: return Mirroring::SINGLE_UPPER;
            case 2: return Mirroring::VERTICAL;
            case 3: return Mirroring::HORIZONTAL;
            default: return Mirroring::INVALID;
        }
    }

    void Mapper001::writeRegister(const std::uint16_t addr, const std::uint8_t value) {
        if (0x8000 <= addr && addr <= 0x9FFF) {
            chrRomBankMode_ = (value & 0b10000) >> 4;
            prgRomBankMode_ = (value & 0b01100) >> 2;
            nametableMode_ = value & 0b00011;
        } else if (0xA000 <= addr && addr <= 0xBFFF) {
            chrRomIndex0_ = value & 0b11111;
        } else if (0xC000 <= addr && addr <= 0xDFFF) {
            chrRomIndex1_ = value & 0b11111;
        } else {
            prgRomIndex_ = value & 0b01111;
            // TODO:
            // MMC1A:
            // 0: fixed bank affects A17..A14
            // 1: fixed bank only affects A16..A14, bit 3 directly controls A17 across the entire $8000-$FFFF address range
            prgRamDisabled_ = (subMapper_ == 2) && ((value >> 4) & 1);
        }
    }
}
