#include "core/ppu.h"
#include "core/bus.h"

namespace nes_core {
    constexpr std::array<std::uint32_t, 64> PALETTE = {
        0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
        0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
        0xADADAD, 0x155FD9, 0x4240FF, 0x7527FE, 0xA01ACC, 0xB71E7B, 0xB53120, 0x994E00,
        0x6B6D00, 0x388700, 0x0C9300, 0x008F32, 0x007C8D, 0x000000, 0x000000, 0x000000,
        0xFFFEFF, 0x64B0FF, 0x9290FF, 0xC676FF, 0xF36AFF, 0xFE6ECC, 0xFE8170, 0xEA9E22,
        0xBCBE00, 0x88D800, 0x5CE430, 0x45E082, 0x48CDDE, 0x4F4F4F, 0x000000, 0x000000,
        0xFFFEFF, 0xC0DFFF, 0xD3D2FF, 0xE8C8FF, 0xFBC2FF, 0xFEC4EA, 0xFECCC5, 0xF7D8A5,
        0xE4E594, 0xCFEF96, 0xBDF4AB, 0xB3F3CC, 0xB5EBF2, 0xB8B8B8, 0x000000, 0x000000,
    };

    std::uint8_t& PPURegisters::r(const int index) {
        switch (index) {
            case 0: return ppuControl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDMA;
        }
    }

    std::uint8_t PPURegisters::r(const int index) const {
        switch (index) {
            case 0: return ppuControl;
            case 1: return ppuMask;
            case 2: return ppuStatus;
            case 3: return oamAddr;
            case 4: return oamData;
            case 5: return ppuScroll;
            case 6: return ppuAddr;
            case 7: return ppuData;
            default: return oamDMA;
        }
    }

    int PPURegisters::baseNametableAddr() const {
        switch (ppuControl & 0b00000011) {
            case 0: return 0x2000;
            case 1: return 0x2400;
            case 2: return 0x2800;
            default: return 0x2C00;
        }
    }

    void PPURegisters::reset() {
        ppuControl = 0;
        ppuMask = 0;
        ppuStatus = 0;
        oamAddr = 0;
        oamData = 0;
        ppuScroll = 0;
        ppuAddr = 0;
        ppuData = 0;
        oamDMA = 0;
        v = 0;
        t = 0;
        x = 0;
        w = 0;
        bgPatternShiftLo = 0;
        bgPatternShiftHi = 0;
        bgAttrShiftLo = 0;
        bgAttrShiftHi = 0;
        ppuDataBuffer = 0;
    }

    PPU::PPU(Bus *bus) {
        bus_ = bus;
    }

    void PPU::reset() {
        regs_.reset();
        oam_.fill(0);
        nametable_.fill(0);
        palette_.fill(0);
        frameBuffer_.fill(0);
        scanline_ = 0;
        cycle_ = 0;
        frame_ = 1;
        suppressVBlank_ = false;
        nametableByte_ = 0;
        attrTableByte_ = 0;
        patternTableTileLo_ = 0;
        patternTableTileHi_ = 0;
    }

    std::uint8_t PPU::registerRead(const std::uint16_t addr) {
        const auto index = addr & 0b00000111;
        auto value = regs_.r(index);
        if (index == 2) {  // PPU_STATUS
            // Reading $2002 at the exact cycle VBlank is set suppresses the flag
            if (scanline_ == 241 && cycle_ == 1) {
                suppressVBlank_ = true;
            }
            regs_.clearVBlank();
            regs_.w = 0;
        } else if (index == 4) {  // OAM_DATA
            value = oam_[regs_.oamAddr];
        } else if (index == 7) {  // PPU_DATA
            value = regs_.ppuDataBuffer;
            regs_.ppuDataBuffer = ppuRead(regs_.v & 0x2FFF);
            if ((regs_.v & PPU_MEM_MASK) >= PPU_PALETTE_START) {
                value = ppuRead(regs_.v);
            }
            regs_.v += regs_.vRamIncrement();
        }
        return value;
    }

    void PPU::registerWrite(const std::uint16_t addr, const std::uint8_t value) {
        const auto index = addr & 0b00000111;
        regs_.r(index) = value;
        if (index == 0) { // PPU_CTRL
            regs_.t = (regs_.t & 0xF3FF) | ((value & 0x03) << 10);
        } else if (index == 4) {  // OAM_DATA
            oam_[regs_.oamAddr++] = value;
        } else if (index == 5) {  // PPU_SCROLL
            if (regs_.w == 0) {
                regs_.w = 1;
                regs_.t = (regs_.t & 0xFFE0) | (value >> 3);
                regs_.x = value & 0b00000111;
            } else {
                regs_.w = 0;
                regs_.t = (regs_.t & 0x8C1F) | ((value & 0x07) << 12);
                regs_.t = (regs_.t & 0xFC1F) | ((value & 0xF8) << 2);
            }
        } else if (index == 6) {  // PPU_ADDR
            if (regs_.w == 0) {
                regs_.w = 1;
                regs_.t = (regs_.t & 0x00FF) | ((value & 0x3F) << 8);
            } else {
                regs_.w = 0;
                regs_.t = (regs_.t & 0xFF00) | value;
                regs_.v = regs_.t;
            }
        } else if (index == 7) {  // PPU_DATA
            ppuWrite(regs_.v, value);
            regs_.v += regs_.vRamIncrement();
        }
    }

    std::uint16_t PPU::mirrorNametableAddr(std::uint16_t addr) const {
        switch (mirroring_) {
            case Mirroring::HORIZONTAL:
                if ((0x0400 <= addr && addr < 0x0800) || (0x0C00 <= addr && addr < 0x1000)) {
                    addr -= 0x0400;
                }
                break;
            case Mirroring::VERTICAL:
                if (0x0800 <= addr && addr < 0x1000) {
                    addr -= 0x0800;
                }
                break;
        }
        return addr;
    }

    std::uint16_t PPU::mirrorPaletteAddr(std::uint16_t addr) {
        addr &= 0x1F;
        if ((addr & 0x13) == 0x10) {
            addr &= ~0x10;
        }
        return addr;
    }

    void PPU::fetchNametableByte() {
        // 10 NN YYYYY XXXXX: $2000 + ($400 x N) + ($20 x Y + X)
        nametableByte_ = ppuRead(0x2000 | (regs_.v & 0x0FFF));
    }

    void PPU::fetchAttributeTableByte() {
        // Attribute tables are at the end of each nametable.
        // 10 NN 1111 YYY XXX: $2000 + ($20 x 30) + ($10 x Y + X)
        const auto& v = regs_.v;
        attrTableByte_ = ppuRead(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
    }

    void PPU::fetchPatternTableTileLo() {
        const auto fineY = (regs_.v >> 12) & 0x07;
        const auto patternTableAddr = regs_.backgroundPatternTableAddr();
        const auto addr = patternTableAddr + (nametableByte_ * 16) + fineY;
        patternTableTileLo_ = ppuRead(addr);
    }

    void PPU::fetchPatternTableTileHi() {
        const auto fineY = (regs_.v >> 12) & 0x07;
        const auto patternTableAddr = regs_.backgroundPatternTableAddr();
        const auto addr = patternTableAddr + (nametableByte_ * 16) + 8 + fineY;
        patternTableTileHi_ = ppuRead(addr);
    }

    void PPU::updateShiftRegisters() {
        regs_.bgPatternShiftLo = (regs_.bgPatternShiftLo & 0xFF00) | patternTableTileLo_;
        regs_.bgPatternShiftHi = (regs_.bgPatternShiftHi & 0xFF00) | patternTableTileHi_;
        // Find tile from coarse: ... .. ...Y. ...X.
        const auto tile = ((regs_.v >> 6) & 2) | ((regs_.v >> 1) & 1);
        const auto paletteBits = (attrTableByte_ >> (tile << 1)) & 0x03;
        regs_.bgAttrShiftLo = (regs_.bgAttrShiftLo & 0xFF00) | ((paletteBits & 0x01) ? 0xFF : 0x00);
        regs_.bgAttrShiftHi = (regs_.bgAttrShiftHi & 0xFF00) | ((paletteBits & 0x02) ? 0xFF : 0x00);
    }

    void PPU::reloadShiftRegistersHi() {
        regs_.bgPatternShiftLo = (regs_.bgPatternShiftLo & 0x00FF) | (patternTableTileLo_ << 8);
        regs_.bgPatternShiftHi = (regs_.bgPatternShiftHi & 0x00FF) | (patternTableTileHi_ << 8);
        // Find tile from coarse: ... .. ...Y. ...X.
        const auto tile = ((regs_.v >> 6) & 2) | ((regs_.v >> 1) & 1);
        const auto paletteBits = (attrTableByte_ >> (tile << 1)) & 0x03;
        regs_.bgAttrShiftLo = (regs_.bgAttrShiftLo & 0x00FF) | ((paletteBits & 0x01) ? 0xFF00 : 0x0000);
        regs_.bgAttrShiftHi = (regs_.bgAttrShiftHi & 0x00FF) | ((paletteBits & 0x02) ? 0xFF00 : 0x0000);
    }

    void PPU::incrementScrollX() {
        // XXXXX + 1
        if ((regs_.v & 0x001F) == 0x001F) {
            // Switch horizontal nametable
            regs_.v &= ~0x001F;
            regs_.v ^= 0x0400;
        } else {
            ++regs_.v;
        }
    }

    void PPU::incrementScrollY() {
        // yyy + 1
        if ((regs_.v & 0x7000) == 0x7000) {
            regs_.v &= ~0x7000;
            // YYYYY + 1
            auto coarseY = (regs_.v & 0x03E0) >> 5;
            if (coarseY == 29) {
                // Switch vertical nametable
                coarseY = 0;
                regs_.v ^= 0x0800;
            } else if (coarseY == 31) {
                coarseY = 0;
            } else {
                ++coarseY;
            }
            regs_.v = (regs_.v & ~0x03E0) | (coarseY << 5);
        } else {
            regs_.v += 0x1000;
        }
    }

    void PPU::copyScrollX() {
        // Copy ... .N ..... XXXXX
        regs_.v = (regs_.v & ~0x041F) | (regs_.t & 0x041F);
    }

    void PPU::copyScrollY() {
        // Copy yyy N. YYYYY .....
        regs_.v = (regs_.v & ~0x7BE0) | (regs_.t & 0x7BE0);
    }

    void PPU::fetchTileData() {
        switch ((cycle_ - 1) % 8) {
            case 0:
                fetchNametableByte();
                break;
            case 2:
                fetchAttributeTableByte();
                break;
            case 4:
                fetchPatternTableTileLo();
                break;
            case 6:
                fetchPatternTableTileHi();
                break;
            case 7:
                if (cycle_ == 328) {
                    reloadShiftRegistersHi();
                } else {
                    updateShiftRegisters();
                }
                incrementScrollX();
                break;
            default:
                break;
        }
    }

    void PPU::renderPixel() {
        const auto y = scanline_, x = cycle_ - 1;
        std::uint8_t bgPattern = 0, bgPalette = 0;
        if (regs_.renderingEnabled() && x < 8 && !regs_.showBackgroundInLeft()) {
            bgPattern = 0;
        } else if (regs_.backgroundRenderingEnabled()) {
            const auto mask = 0x8000 >> regs_.x;
            const auto patternLo = (regs_.bgPatternShiftLo & mask) ? 1 : 0;
            const auto patternHi = (regs_.bgPatternShiftHi & mask) ? 2 : 0;
            bgPattern = patternHi | patternLo;
            if (bgPattern != 0) {
                const auto paletteLo = (regs_.bgAttrShiftLo & mask) ? 1 : 0;
                const auto paletteHi = (regs_.bgAttrShiftHi & mask) ? 2 : 0;
                bgPalette = paletteHi | paletteLo;
            }
        }
        // TODO: Add OAM
        const auto finalPattern = bgPattern;
        const auto finalPalette = bgPalette;
        const auto color = ppuRead(PPU_PALETTE_START + (finalPalette * 4) + finalPattern);
        frameBuffer_[y * 256 + x] = 0xFF000000 | PALETTE[color];
    }

    std::uint8_t PPU::ppuRead(std::uint16_t addr) const {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            return bus_->ppuRead(addr);
        }
        if (addr < PPU_PALETTE_START) {
            if (addr >= PPU_NAMETABLE_MIRRO_START) {
                addr -= PPU_NAMETABLE_SIZE;
            }
            addr -= PPU_NAMETABLE_START;
            addr = mirrorNametableAddr(addr);
            return nametable_[addr];
        }
        return palette_[mirrorPaletteAddr(addr)];
    }

    void PPU::ppuWrite(std::uint16_t addr, const std::uint8_t value) {
        addr &= PPU_MEM_MASK;
        if (addr < CHR_ROM_BANK_SIZE) {
            bus_->ppuWrite(addr, value);
        } else if (addr < PPU_PALETTE_START) {
            if (addr >= PPU_NAMETABLE_MIRRO_START) {
                addr -= PPU_NAMETABLE_SIZE;
            }
            addr -= PPU_NAMETABLE_START;
            addr = mirrorNametableAddr(addr);
            nametable_[addr] = value;
        } else {
            palette_[mirrorPaletteAddr(addr)] = value;
        }
    }

    void PPU::step() {
        const auto renderingEnabled = regs_.renderingEnabled();
        if (0 <= scanline_ && scanline_ < 240) {
            if (renderingEnabled) {
                if (1 <= cycle_ && cycle_ <= 256) {
                    renderPixel();
                    regs_.bgPatternShiftLo <<= 1;
                    regs_.bgPatternShiftHi <<= 1;
                    regs_.bgAttrShiftLo <<= 1;
                    regs_.bgAttrShiftHi <<= 1;
                    fetchTileData();
                    if (cycle_ == 256) {
                        incrementScrollY();
                    }
                } else if (257 <= cycle_ && cycle_ <= 320) {
                    if (cycle_ == 257) {
                        copyScrollX();
                    }
                    // TODO
                } else if (321 <= cycle_ && cycle_ <= 336) {
                    fetchTileData();
                }
            }
        } else if (scanline_ == 241) {
            if (cycle_ == 1) {
                if (!suppressVBlank_) {
                    regs_.setVBlank();
                    if (regs_.vBlankNmiEnabled()) {
                        bus_->cpuNMI();
                    }
                }
                suppressVBlank_ = false;
            }
        } else if (scanline_ == 261) {
            if (cycle_ == 1) {
                regs_.clearVBlank();
            }
            if (renderingEnabled) {
                if (1 <= cycle_ && cycle_ <= 256) {
                    regs_.bgPatternShiftLo <<= 1;
                    regs_.bgPatternShiftHi <<= 1;
                    regs_.bgAttrShiftLo <<= 1;
                    regs_.bgAttrShiftHi <<= 1;
                    fetchTileData();
                    if (cycle_ == 256) {
                        incrementScrollY();
                    }
                } else if (257 <= cycle_ && cycle_ <= 320) {
                    if (cycle_ == 257) {
                        copyScrollX();
                    }
                    if (cycle_ >= 280 && cycle_ <= 304) {
                        copyScrollY();
                    }
                } else if (321 <= cycle_ && cycle_ <= 336) {
                    fetchTileData();
                }
                if (cycle_ == 339 && frame_ % 2 == 1) {
                    cycle_ = -1;
                    scanline_ = 0;
                    ++frame_;
                }
            }
        }
        if (++cycle_ > 340) {
            cycle_ = 0;
            if (++scanline_ > 261) {
                scanline_ = 0;
                ++frame_;
            }
        }
    }

}
