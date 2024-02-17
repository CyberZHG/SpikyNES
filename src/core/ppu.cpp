#include "core/ppu.h"
#include "core/bus.h"

namespace nes_core {
    constexpr std::array<std::uint32_t, 64> PALETTE = {
        0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
        0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
        0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
        0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
        0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
        0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
        0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
        0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000,
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
        sprites_.fill({});
        spriteCount_ = 0;
        sprite0Rendering_ = false;
        scanline_ = 0;
        cycle_ = 0;
        frame_ = 1;
        suppressVBlank_ = false;
        isVBlankNMIOn_ = false;
        isVBlankNMIOnPrev_ = false;
        vBlankNMIPending_ = false;
        nametableByte_ = 0;
        attrTableByte_ = 0;
        patternTableTileLo_ = 0;
        patternTableTileHi_ = 0;
    }

    std::uint8_t PPU::registerRead(const std::uint16_t addr) {
        const auto index = addr & 0b00000111;
        std::uint8_t value = bus_->lastBusValue();
        if (index == 2) {  // PPU_STATUS
            if (scanline_ == 241) {
                if (cycle_ == 1) {
                    suppressVBlank_ = true;
                } else if (cycle_ == 2 || cycle_ == 3) {
                    bus_->cpuNMIClear();
                    vBlankNMIPending_ = false;
                }
            }
            value = (regs_.ppuStatus & 0xE0) | (bus_->lastBusValue() & 0x1F);
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
            if (!regs_.vBlankNmiEnabled()) {
                if (scanline_ == 241) {
                    if (cycle_ == 1) {
                        suppressVBlank_ = true;
                    } else if (cycle_ == 2 || cycle_ == 3) {
                        bus_->cpuNMIClear();
                        vBlankNMIPending_ = false;
                    }
                }
            }
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

    void PPU::updateVBlankNMI() {
        // See: https://forums.nesdev.org/viewtopic.php?t=20769
        if (vBlankNMIPending_) {
            vBlankNMIPending_ = false;
            bus_->cpuNMI();
        }
        isVBlankNMIOnPrev_ = isVBlankNMIOn_;
        isVBlankNMIOn_ = regs_.vBlank() && regs_.vBlankNmiEnabled();
        if (!isVBlankNMIOnPrev_ && isVBlankNMIOn_) {
            vBlankNMIPending_ = true;
        }
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

    void PPU::fetchBackgroundTileData() {
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
        // Background
        std::uint8_t bgPattern = 0, bgPalette = 0;
        if (regs_.bothRenderingEnabled() && x < 8 && !regs_.showBackgroundInLeft()) {
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
        // OAM
        std::uint8_t spritePattern = 0, spritePalette = 0, spriteY = 0;
        bool behindBackground = false, isSprite0 = false;
        if (regs_.bothRenderingEnabled() && x < 8 && !regs_.showSpritesInLeft()) {
            spritePattern = 0;
        } else if (regs_.spriteRenderingEnabled()) {
            for (int i = 0; i < spriteCount_; ++i) {
                if (sprites_[i].x == 0) {
                    const auto lo = (sprites_[i].patternLo & 0x80) ? 1 : 0;
                    const auto hi = (sprites_[i].patternHi & 0x80) ? 2 : 0;
                    spritePattern = hi | lo;
                    spritePalette = sprites_[i].palette();
                    if (spritePattern != 0) {
                        spriteY = sprites_[i].y;
                        behindBackground = sprites_[i].behindBackground();
                        isSprite0 = i == 0 && sprite0Rendering_;
                        break;
                    }
                }
            }
        }
        // Sprite 0 Hit
        if (isSprite0 && x != 255 && spriteY != 0xFF && bgPattern != 0 && spritePattern != 0) {
            regs_.setSprite0Hit();
        }
        // Ordering
        auto finalPattern = 0, finalPalette = 0;
        if (bgPattern != 0 && spritePattern == 0) {
            finalPattern = bgPattern;
            finalPalette = bgPalette;
        } else if (bgPattern == 0 && spritePattern != 0) {
            finalPattern = spritePattern;
            finalPalette = spritePalette;
        } else if (bgPattern != 0 && spritePattern != 0) {
            if (behindBackground) {
                finalPattern = bgPattern;
                finalPalette = bgPalette;
            } else {
                finalPattern = spritePattern;
                finalPalette = spritePalette;
            }
        }
        const auto color = ppuRead(PPU_PALETTE_START + (finalPalette * 4) + finalPattern);
        frameBuffer_[y * 256 + x] = 0xFF000000 | PALETTE[color];
    }

    void PPU::evaluateSprites() {
        const auto spriteHeight = regs_.spriteSize();
        auto scanline = static_cast<std::uint8_t>(scanline_);
        if (scanline_ == 261) {
            scanline = 0xFF;
        }
        spriteCount_ = 0;
        sprite0Rendering_ = false;
        for (int i = 0; i < 64; ++i) {
            const auto y = oam_[i * 4];
            const auto tile = oam_[i * 4 + 1];
            const auto attribute = oam_[i * 4 + 2];
            const auto x = oam_[i * 4 + 3];
            if (const auto row = scanline - y; 0 <= row && row < spriteHeight) {
                if (spriteCount_ < 8) {
                    if (i == 0) {
                        sprite0Rendering_ = true;
                    }
                    sprites_[spriteCount_].y = y;
                    sprites_[spriteCount_].tile = tile;
                    sprites_[spriteCount_].attribute = attribute;
                    sprites_[spriteCount_].x = x;
                    ++spriteCount_;
                } else {
                    if (scanline_ != 261) {
                        regs_.setSpriteOverflow();
                    }
                    break;
                }
            }
        }
    }

    void PPU::fetchSprites() {
        const auto spriteHeight = regs_.spriteSize();
        auto scanline = static_cast<std::uint8_t>(scanline_);
        if (scanline_ == 261) {
            scanline = 0xFF;
        }
        for (int i = 0; i < spriteCount_; ++i) {
            int row = scanline - sprites_[i].y;
            if (sprites_[i].verticalFlip()) {
                row = spriteHeight - 1 - row;
            }
            auto patternTable = 0;
            auto tile = sprites_[i].tile;
            if (spriteHeight == 8) {
                patternTable = regs_.spritePatternTableAddr();
            } else {
                patternTable = (tile & 1) ? 0x1000 : 0x0000;
                tile &= 0xFE;
                if (row >= 8) {
                    ++tile;
                    row -= 8;
                }
            }
            const auto patternAddr = patternTable + tile * 16 + row;
            sprites_[i].patternLo = ppuRead(patternAddr);
            sprites_[i].patternHi = ppuRead(patternAddr + 8);
            if (sprites_[i].horizontalFlip()) {
                sprites_[i].patternLo = reverseBits(sprites_[i].patternLo);
                sprites_[i].patternHi = reverseBits(sprites_[i].patternHi);
            }
        }
    }

    void PPU::shiftSprites() {
        for (int i = 0; i < spriteCount_; ++i) {
            if (sprites_[i].x > 0) {
                --sprites_[i].x;
            } else {
                sprites_[i].patternLo <<= 1;
                sprites_[i].patternHi <<= 1;
            }
        }
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
        updateVBlankNMI();
        if (0 <= scanline_ && scanline_ < 240) {
            if (renderingEnabled) {
                if (1 <= cycle_ && cycle_ <= 256) {
                    renderPixel();
                    shiftSprites();
                    regs_.bgPatternShiftLo <<= 1;
                    regs_.bgPatternShiftHi <<= 1;
                    regs_.bgAttrShiftLo <<= 1;
                    regs_.bgAttrShiftHi <<= 1;
                    fetchBackgroundTileData();
                    if (cycle_ == 256) {
                        incrementScrollY();
                    }
                } else if (257 <= cycle_ && cycle_ <= 320) {
                    if (cycle_ == 257) {
                        regs_.oamAddr = 0;
                        copyScrollX();
                        evaluateSprites();
                        fetchSprites();
                    }
                } else if (321 <= cycle_ && cycle_ <= 336) {
                    fetchBackgroundTileData();
                }
            }
        } else if (scanline_ == 241) {
            if (cycle_ == 1) {
                if (!suppressVBlank_) {
                    regs_.setVBlank();
                }
                suppressVBlank_ = false;
            }
        } else if (scanline_ == 261) {
            if (cycle_ == 1) {
                regs_.ppuStatus &= ~0xE0;
            }
            if (renderingEnabled) {
                if (1 <= cycle_ && cycle_ <= 256) {
                    regs_.bgPatternShiftLo <<= 1;
                    regs_.bgPatternShiftHi <<= 1;
                    regs_.bgAttrShiftLo <<= 1;
                    regs_.bgAttrShiftHi <<= 1;
                    fetchBackgroundTileData();
                    if (cycle_ == 256) {
                        incrementScrollY();
                    }
                } else if (257 <= cycle_ && cycle_ <= 320) {
                    if (cycle_ == 257) {
                        copyScrollX();
                        evaluateSprites();
                        fetchSprites();
                    }
                    if (cycle_ >= 280 && cycle_ <= 304) {
                        copyScrollY();
                    }
                } else if (321 <= cycle_ && cycle_ <= 336) {
                    fetchBackgroundTileData();
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
