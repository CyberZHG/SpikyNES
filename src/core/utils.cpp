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

    bool writePPM(const std::string& path, const int width, const int height, const std::uint8_t* data, const bool binary) {
        std::ofstream file(path, binary ? std::ios::binary : std::ios::out);
        if (!file) {
            return false;
        }

        if (binary) {
            file << "P6\n" << width << " " << height << "\n255\n";
            file.write(reinterpret_cast<const char*>(data), width * height * 3);
        } else {
            file << "P3\n" << width << " " << height << "\n255\n";
            for (int i = 0; i < width * height; ++i) {
                file << static_cast<int>(data[i * 3]) << ' '
                     << static_cast<int>(data[i * 3 + 1]) << ' '
                     << static_cast<int>(data[i * 3 + 2]);
                file << ((i + 1) % width == 0 ? '\n' : ' ');
            }
        }
        return file.good();
    }

    bool writePPM(const std::string& path, const std::array<std::uint32_t, 256 * 240>& data, const bool binary) {
        std::vector<std::uint8_t> rgb(data.size() * 3);
        for (int i = 0; i < data.size(); ++i) {
            rgb[i * 3] = static_cast<std::uint8_t>((data[i] & 0x00FF0000) >> 16);
            rgb[i * 3 + 1] = static_cast<std::uint8_t>((data[i] & 0x0000FF00) >> 8);
            rgb[i * 3 + 2] = static_cast<std::uint8_t>(data[i] & 0x000000FF);
        }
        return writePPM(path, 256, 240, rgb.data(), binary);
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

    std::vector<std::uint8_t> readPPM(const std::string& path, int& width, int& height) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return {};
        }
        std::string magic;
        file >> magic;
        if (magic != "P3" && magic != "P6") {
            return {};
        }
        const bool binary = (magic == "P6");
        file >> width >> height;
        int maxVal;
        file >> maxVal;
        file.get();
        std::vector<std::uint8_t> data(width * height * 3);
        if (binary) {
            file.read(reinterpret_cast<char*>(data.data()), width * height * 3);
        } else {
            for (int i = 0; i < width * height * 3; ++i) {
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
