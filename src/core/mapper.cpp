#include "core/mapper.h"
#include "core/mapper/mapper_000.h"
#include "core/mapper/mapper_001.h"
#include "core/mapper/mapper_002.h"
#include "core/mapper/mapper_003.h"

namespace nes_core {
    std::unique_ptr<Mapper> Mapper::create(
        const std::uint16_t mapperId,
        const std::uint8_t prgRomBanks,
        const std::uint8_t prgRamBanks,
        const std::uint8_t chrRomBanks,
        std::vector<std::uint8_t>* prgRom,
        std::vector<std::uint8_t>* prgRam,
        std::vector<std::uint8_t>* chrRom,
        std::vector<std::uint8_t>* chrRam) {
        std::unique_ptr<Mapper> mapper = nullptr;
        switch (mapperId) {
            case 0: mapper = std::make_unique<Mapper000>(); break;
            case 1: mapper = std::make_unique<Mapper001>(); break;
            case 2: mapper = std::make_unique<Mapper002>(); break;
            case 3: mapper = std::make_unique<Mapper003>(); break;
            default: break;
        }
        if (mapper != nullptr) {
            mapper->prgRomBanks_ = prgRomBanks;
            mapper->prgRamBanks_ = prgRamBanks;
            mapper->chrRomBanks_ = chrRomBanks;
            mapper->prgRom_ = prgRom;
            mapper->prgRam_ = prgRam;
            mapper->chrRom_ = chrRom;
            mapper->chrRam_ = chrRam;
        }
        return mapper;
    }
}
