#include "test_image_utils.h"

#include <span>

#include "tiny_png/tiny_png.h"

using namespace tiny_png;

bool writePNG(const std::string& path, const int width, const int height, const std::uint8_t* data) {
    const std::span<const std::uint8_t> pixels(data, static_cast<std::size_t>(width) * height * 3);
    return save_image(path, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), pixels,
                      ColorType::RGB);
}

bool writePNG(const std::string& path, const std::array<std::uint32_t, 256 * 240>& data) {
    std::vector<std::uint8_t> rgb(data.size() * 3);
    for (std::size_t i = 0; i < data.size(); ++i) {
        rgb[i * 3] = static_cast<std::uint8_t>((data[i] & 0x00FF0000) >> 16);
        rgb[i * 3 + 1] = static_cast<std::uint8_t>((data[i] & 0x0000FF00) >> 8);
        rgb[i * 3 + 2] = static_cast<std::uint8_t>(data[i] & 0x000000FF);
    }
    return writePNG(path, 256, 240, rgb.data());
}

std::vector<std::uint8_t> readPNG(const std::string& path, int& width, int& height) {
    const PNGData image = load_image(path);
    if (!image.error.empty()) {
        return {};
    }
    width = static_cast<int>(image.width);
    height = static_cast<int>(image.height);
    const std::size_t bytesPerPixel = getBytesPerPixel(image.colorType);
    const std::size_t pixelCount = image.width * image.height;
    std::vector<std::uint8_t> rgb(pixelCount * 3);
    for (std::size_t i = 0; i < pixelCount; ++i) {
        rgb[i * 3] = image.buffer[i * bytesPerPixel];
        rgb[i * 3 + 1] = image.buffer[i * bytesPerPixel + 1];
        rgb[i * 3 + 2] = image.buffer[i * bytesPerPixel + 2];
    }
    return rgb;
}
