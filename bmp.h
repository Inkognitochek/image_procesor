#ifndef GRAPHPROCEXAMPLES_BMP_H
#define GRAPHPROCEXAMPLES_BMP_H

#include "image.h"
#include <cstdint>

#pragma pack(push, 1)

struct BmpHeader {
    uint16_t sign;
    uint32_t size;
    uint16_t reserved_a;
    uint16_t reserved_b;
    uint32_t offset;
};

struct DibHeader {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};

#pragma pack(pop)

Image ReadImage(const char* fn);
void WriteImage(const char* fn, const Image& img);

#endif  // GRAPHPROCEXAMPLES_BMP_H