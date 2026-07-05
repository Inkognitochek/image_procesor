#include "bmp.h"

#include <fstream>
#include <stdexcept>
#include <cmath>
#include <cstdint>

namespace {
constexpr int Bits24 = 24;
constexpr double ByteToDouble = 255.0;
constexpr uint16_t BmpSignature = 0x4D42;
}  // namespace

Image ReadImage(const char* fn) {
    std::ifstream file;
    file.open(fn, std::ios_base::in | std::ios_base::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(fn));
    }

    BmpHeader bmp_header;
    file.read(reinterpret_cast<char*>(&bmp_header), sizeof(bmp_header));

    static const uint16_t SIGNATURE = 0x4d42;
    if (bmp_header.sign != SIGNATURE) {
        throw std::runtime_error("Not a BMP file: wrong signature");
    }

    DibHeader dib_header;
    file.read(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));

    if (dib_header.bit_count != Bits24) {
        throw std::runtime_error("Only 24-bit BMP is supported");
    }

    int width = dib_header.width;
    int height = std::abs(dib_header.height);
    Image img(width, height);

    int padding = (4 - (width * 3 % 4)) % 4;

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            unsigned char bgr[3];

            if (!file.read(reinterpret_cast<char*>(bgr), 3)) {
                throw std::runtime_error("Error reading pixel data at x=" + std::to_string(x) +
                                         " y=" + std::to_string(y));
            }

            img.GetPixel(x, y).r = static_cast<double>(bgr[2]) / ByteToDouble;
            img.GetPixel(x, y).g = static_cast<double>(bgr[1]) / ByteToDouble;
            img.GetPixel(x, y).b = static_cast<double>(bgr[0]) / ByteToDouble;
        }
        file.seekg(padding, std::ios::cur);
    }

    return img;
}

void WriteImage(const char* fn, const Image& img) {
    std::ofstream file(fn, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Can't create output file");
    }

    int width = static_cast<int>(img.GetWidth());
    int height = static_cast<int>(img.GetHeight());
    int padding = (4 - (width * 3 % 4)) % 4;

    BmpHeader bmp_header;
    bmp_header.sign = BmpSignature;
    bmp_header.offset = sizeof(BmpHeader) + sizeof(DibHeader);
    bmp_header.reserved_a = 0;
    bmp_header.reserved_b = 0;
    bmp_header.size = bmp_header.offset + (width * 3 + padding) * height;

    DibHeader dib_header;
    dib_header.header_size = sizeof(DibHeader);
    dib_header.width = width;
    dib_header.height = height;
    dib_header.planes = 1;
    dib_header.bit_count = Bits24;
    dib_header.compression = 0;
    dib_header.size_image = (width * 3 + padding) * height;
    dib_header.x_pixels_per_meter = 0;
    dib_header.y_pixels_per_meter = 0;
    dib_header.colors_used = 0;
    dib_header.colors_important = 0;

    file.write(reinterpret_cast<char*>(&bmp_header), sizeof(bmp_header));
    file.write(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const Pixel& p = img.GetPixel(x, y);
            unsigned char bgr[3];

            bgr[0] = static_cast<unsigned char>(p.b * ByteToDouble);
            bgr[1] = static_cast<unsigned char>(p.g * ByteToDouble);
            bgr[2] = static_cast<unsigned char>(p.r * ByteToDouble);

            file.write(reinterpret_cast<char*>(bgr), 3);
        }
        unsigned char pad_byte = 0;
        for (int i = 0; i < padding; ++i) {
            file.write(reinterpret_cast<char*>(&pad_byte), 1);
        }
    }
}