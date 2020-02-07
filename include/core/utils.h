#ifndef SPIKYNES_UTILS_H
#define SPIKYNES_UTILS_H

#include <cstdint>
#include <string>
#include <vector>

namespace nes_core {
    inline bool getBit(const std::uint8_t& byte, const int bit) { return byte & (1 << bit); }
    inline void setBit(std::uint8_t& byte, const int bit) { byte |= 1 << bit; }
    inline void clearBit(std::uint8_t& byte, const int bit) { byte &= ~(1 << bit); }
    inline void updateBit(std::uint8_t& byte, const int bit, const std::uint8_t flag) { byte = (byte & ~(1 << bit)) | flag; }
    inline void updateBit(std::uint8_t& byte, const int bit, const bool flag) {
        updateBit(byte, bit, static_cast<std::uint8_t>(flag << bit));
    }

    /**
     * Write a PGM (grayscale) image file.
     * @param path Output file path
     * @param width Image width
     * @param height Image height
     * @param data Grayscale pixel data (1 byte per pixel, row-major)
     * @param binary true for binary format (P5), false for ASCII format (P2)
     * @return true on success
     */
    bool writePGM(const std::string& path, int width, int height, const std::uint8_t* data, bool binary = true);

    /**
     * Write a PPM (color) image file.
     * @param path Output file path
     * @param width Image width
     * @param height Image height
     * @param data RGB pixel data (3 bytes per pixel: R, G, B, row-major)
     * @param binary true for binary format (P6), false for ASCII format (P3)
     * @return true on success
     */
    bool writePPM(const std::string& path, int width, int height, const std::uint8_t* data, bool binary = true);

    /**
     * Read a PGM (grayscale) image file. Supports P2 (ASCII) and P5 (binary).
     * @param path Input file path
     * @param width Output image width
     * @param height Output image height
     * @return Grayscale pixel data (1 byte per pixel), empty on failure
     */
    std::vector<std::uint8_t> readPGM(const std::string& path, int& width, int& height);

    /**
     * Read a PPM (color) image file. Supports P3 (ASCII) and P6 (binary).
     * @param path Input file path
     * @param width Output image width
     * @param height Output image height
     * @return RGB pixel data (3 bytes per pixel: R, G, B), empty on failure
     */
    std::vector<std::uint8_t> readPPM(const std::string& path, int& width, int& height);
}

#endif //SPIKYNES_UTILS_H