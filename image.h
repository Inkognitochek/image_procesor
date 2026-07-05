//
// Created by Лось Егор Дмитриевич on 17.03.26.
//

#ifndef GRAPHPROCEXAMPLES_IMAGE_H
#define GRAPHPROCEXAMPLES_IMAGE_H

#include <vector>
#include <cstddef>

struct Pixel {
    double r = 0;
    double g = 0;
    double b = 0;
};

class Image {
public:
    Image(size_t width, size_t height) : width_(width), height_(height), data_(height, std::vector<Pixel>(width)) {
    }

    size_t GetWidth() const {
        return width_;
    }

    size_t GetHeight() const {
        return height_;
    }

    Pixel& GetPixel(size_t x, size_t y) {
        return data_[y][x];
    }

    const Pixel& GetPixel(size_t x, size_t y) const {
        return data_[y][x];
    }

    void UpdateData(size_t new_w, size_t new_h, std::vector<std::vector<Pixel>>&& new_data) {
        width_ = new_w;
        height_ = new_h;
        data_ = std::move(new_data);
    }

private:
    size_t width_;
    size_t height_;
    std::vector<std::vector<Pixel>> data_;
};

#endif  // GRAPHPROCEXAMPLES_IMAGE_H