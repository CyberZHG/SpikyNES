#ifndef SPIKYNES_TEST_IMAGE_UTILS_H
#define SPIKYNES_TEST_IMAGE_UTILS_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

/**
 * Write a PNG (color) image file using TinyPNG.
 * @param path Output file path
 * @param width Image width
 * @param height Image height
 * @param data RGB pixel data (3 bytes per pixel: R, G, B, row-major)
 * @return true on success
 */
bool writePNG(const std::string& path, int width, int height, const std::uint8_t* data);

/**
 * Write a frame buffer to a PNG (color) image file.
 * @param path Output file path
 * @param data ARGB pixel data (AARRGGBB)
 * @return true on success
 */
bool writePNG(const std::string& path, const std::array<std::uint32_t, 256 * 240>& data);

/**
 * Read a PNG (color) image file using TinyPNG.
 * @param path Input file path
 * @param width Output image width
 * @param height Output image height
 * @return RGB pixel data (3 bytes per pixel: R, G, B), empty on failure
 */
std::vector<std::uint8_t> readPNG(const std::string& path, int& width, int& height);

#endif //SPIKYNES_TEST_IMAGE_UTILS_H
