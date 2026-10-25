#include "core/utils.h"
#include <fstream>

namespace nes_core {
    std::uint8_t reverseBits(std::uint8_t value) {
        value = (value >> 4) | (value << 4);
        value = ((value & 0xCC) >> 2) | ((value & 0x33) << 2);
        value = ((value & 0xAA) >> 1) | ((value & 0x55) << 1);
        return value;
    }

    bool writePGM(const std::string& path, const int width, const int height, const std::uint8_t* data, const bool binary) {
        std::ofstream file(path, binary ? std::ios::binary : std::ios::out);
        if (!file) {
            return false;
        }

        if (binary) {
            file << "P5\n" << width << " " << height << "\n255\n";
            file.write(reinterpret_cast<const char*>(data), width * height);
        } else {
            file << "P2\n" << width << " " << height << "\n255\n";
            for (int i = 0; i < width * height; ++i) {
                file << static_cast<int>(data[i]);
                file << ((i + 1) % width == 0 ? '\n' : ' ');
            }
        }
        return file.good();
    }

    std::vector<std::uint8_t> readPGM(const std::string& path, int& width, int& height) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return {};
        }
        std::string magic;
        file >> magic;
        if (magic != "P2" && magic != "P5") {
            return {};
        }
        const bool binary = (magic == "P5");
        file >> width >> height;
        int maxVal;
        file >> maxVal;
        file.get();
        std::vector<std::uint8_t> data(width * height);
        if (binary) {
            file.read(reinterpret_cast<char*>(data.data()), width * height);
        } else {
            for (int i = 0; i < width * height; ++i) {
                int val;
                file >> val;
                data[i] = static_cast<std::uint8_t>(val);
            }
        }
        if (!file) {
            return {};
        }
        return data;
    }
}
