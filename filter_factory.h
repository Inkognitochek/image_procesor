//
// Created by Лось Егор Дмитриевич on 05.03.26.
//

#ifndef GRAPHPROCEXAMPLES_FILTER_FACTORY_H
#define GRAPHPROCEXAMPLES_FILTER_FACTORY_H

#include "filter.h"
#include "parser_cmd_args.h"
#include <algorithm>
#include <map>
#include <functional>
#include <random>
#include <string>
#include <stdexcept>
#include <cmath>

namespace {
constexpr double GrayR = 0.299;
constexpr double GrayG = 0.587;
constexpr double GrayB = 0.114;
constexpr double SharpCenter = 5.0;
constexpr int Bits24 = 24;
constexpr double ByteToDouble = 255.0;
constexpr uint16_t BmpSignature = 0x4D42;
}  // namespace

class GrayscaleFilter : public BaseFilter {
public:
    void Apply(Image& img) const override {
        for (size_t y = 0; y < img.GetHeight(); ++y) {
            for (size_t x = 0; x < img.GetWidth(); ++x) {
                Pixel& p = img.GetPixel(x, y);
                double gray = GrayR * p.r + GrayG * p.g + GrayB * p.b;
                p.r = p.g = p.b = gray;
            }
        }
    }
};

class NegativeFilter : public BaseFilter {
public:
    void Apply(Image& img) const override {
        for (size_t y = 0; y < img.GetHeight(); ++y) {
            for (size_t x = 0; x < img.GetWidth(); ++x) {
                Pixel& p = img.GetPixel(x, y);
                p.r = 1.0 - p.r;
                p.g = 1.0 - p.g;
                p.b = 1.0 - p.b;
            }
        }
    }
};

class SharpeningFilter : public BaseFilter {
public:
    void Apply(Image& img) const override {
        Image old = img;
        int w = static_cast<int>(img.GetWidth());
        int h = static_cast<int>(img.GetHeight());
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                auto get = [&](int ix, int iy) {
                    return old.GetPixel(std::clamp(ix, 0, w - 1), std::clamp(iy, 0, h - 1));
                };
                Pixel res;
                res.r =
                    SharpCenter * get(x, y).r - get(x - 1, y).r - get(x + 1, y).r - get(x, y - 1).r - get(x, y + 1).r;
                res.g =
                    SharpCenter * get(x, y).g - get(x - 1, y).g - get(x + 1, y).g - get(x, y - 1).g - get(x, y + 1).g;
                res.b =
                    SharpCenter * get(x, y).b - get(x - 1, y).b - get(x + 1, y).b - get(x, y - 1).b - get(x, y + 1).b;
                img.GetPixel(x, y) = {std::clamp(res.r, 0.0, 1.0), std::clamp(res.g, 0.0, 1.0),
                                      std::clamp(res.b, 0.0, 1.0)};
            }
        }
    }
};

class CropFilter : public BaseFilter {
public:
    explicit CropFilter(size_t w, size_t h) : w_(w), h_(h) {
    }

    void Apply(Image& img) const override {
        size_t rw = std::min(w_, img.GetWidth());
        size_t rh = std::min(h_, img.GetHeight());
        std::vector<std::vector<Pixel>> data(rh, std::vector<Pixel>(rw));
        for (size_t y = 0; y < rh; ++y) {
            for (size_t x = 0; x < rw; ++x) {
                data[y][x] = img.GetPixel(x, y);
            }
        }
        img.UpdateData(rw, rh, std::move(data));
    }

private:
    size_t w_, h_;
};

class EdgeDetectionFilter : public BaseFilter {
public:
    explicit EdgeDetectionFilter(double threshold) : threshold_(threshold) {
    }

    void Apply(Image& img) const override {
        for (size_t y = 0; y < img.GetHeight(); ++y) {
            for (size_t x = 0; x < img.GetWidth(); ++x) {
                Pixel& p = img.GetPixel(x, y);
                double gray = GrayR * p.r + GrayG * p.g + GrayB * p.b;
                p.r = p.g = p.b = gray;
            }
        }

        Image old = img;
        int w = static_cast<int>(img.GetWidth());
        int h = static_cast<int>(img.GetHeight());
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                auto get = [&](int ix, int iy) {
                    return old.GetPixel(std::clamp(ix, 0, w - 1), std::clamp(iy, 0, h - 1)).r;
                };

                double res = 4 * get(x, y) - get(x - 1, y) - get(x + 1, y) - get(x, y - 1) - get(x, y + 1);

                double color = (res > threshold_) ? 1.0 : 0.0;
                img.GetPixel(x, y) = {color, color, color};
            }
        }
    }

private:
    double threshold_;
};

class BlurFilter : public BaseFilter {
public:
    explicit BlurFilter(double sigma) : sigma_(sigma) {
    }

    void Apply(Image& img) const override {
        int w = static_cast<int>(img.GetWidth());
        int h = static_cast<int>(img.GetHeight());
        int radius = static_cast<int>(std::ceil(sigma_ * 3));
        std::vector<double> kernel(2 * radius + 1);

        double sum = 0;
        for (int i = -radius; i <= radius; ++i) {
            kernel[i + radius] = std::exp(-(i * i) / (2 * sigma_ * sigma_));
            sum += kernel[i + radius];
        }
        for (double& v : kernel) {
            v /= sum;
        }

        Image temp = img;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel res = {0, 0, 0};
                for (int i = -radius; i <= radius; ++i) {
                    int nx = std::clamp(x + i, 0, w - 1);
                    Pixel p = temp.GetPixel(nx, y);
                    double k = kernel[i + radius];
                    res.r += p.r * k;
                    res.g += p.g * k;
                    res.b += p.b * k;
                }
                img.GetPixel(x, y) = res;
            }
        }

        temp = img;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel res = {0, 0, 0};
                for (int i = -radius; i <= radius; ++i) {
                    int ny = std::clamp(y + i, 0, h - 1);
                    Pixel p = temp.GetPixel(x, ny);
                    double k = kernel[i + radius];
                    res.r += p.r * k;
                    res.g += p.g * k;
                    res.b += p.b * k;
                }
                img.GetPixel(x, y) = res;
            }
        }
    }

private:
    double sigma_;
};

class GlassFilter : public BaseFilter {
public:
    GlassFilter(double amplitude, double frequency, int noise_radius)
        : amplitude_(amplitude), frequency_(frequency), noise_radius_(noise_radius) {
    }

    void Apply(Image& img) const override {
        Image original = img;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(-noise_radius_, noise_radius_);

        int width = static_cast<int>(img.GetWidth());
        int height = static_cast<int>(img.GetHeight());

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double dx = amplitude_ * std::sin(y * frequency_);
                double dy = amplitude_ * std::cos(x * frequency_);

                int nx = x + static_cast<int>(dx) + dist(gen);
                int ny = y + static_cast<int>(dy) + dist(gen);

                nx = std::clamp(nx, 0, width - 1);
                ny = std::clamp(ny, 0, height - 1);

                img.GetPixel(x, y) = original.GetPixel(nx, ny);
            }
        }
    }

private:
    double amplitude_;
    double frequency_;
    int noise_radius_;
};

class FilterFactory {
public:
    using CreatorFunc = std::function<BaseFilter*(const FilterDescriptor&)>;

    static BaseFilter* Create(const FilterDescriptor& d) {
        static std::map<std::string, CreatorFunc> registry = {
            {"-neg", [](const auto&) { return new NegativeFilter(); }},
            {"-gs", [](const auto&) { return new GrayscaleFilter(); }},
            {"-crop",
             [](const FilterDescriptor& d) {
                 return new CropFilter(std::stoul(d.params[0]), std::stoul(d.params[1]));
             }},
            {"-edge",
             [](const FilterDescriptor& d) {
                 if (d.params.empty()) {
                     throw std::runtime_error("Edge needs threshold");
                 }
                 return new EdgeDetectionFilter(std::stod(d.params[0]));
             }},
            {"-blur",
             [](const FilterDescriptor& d) {
                 if (d.params.empty()) {
                     throw std::runtime_error("Blur needs sigma");
                 }
                 return new BlurFilter(std::stod(d.params[0]));
             }},
            {"-sharp", [](const FilterDescriptor&) { return new SharpeningFilter(); }},
            {"-glass", [](const FilterDescriptor& d) {
                 if (d.params.size() < 3) {
                     throw std::runtime_error("Glass needs 3 params");
                 }
                 double amplitude = std::stod(d.params[0]);
                 double frequency = std::stod(d.params[1]);
                 int noise_radius = static_cast<int>(std::stoul(d.params[2]));
                 return new GlassFilter(amplitude, frequency, noise_radius);
             }}};

        if (registry.count(d.name)) {
            return registry[d.name](d);
        }
        throw std::runtime_error("Unknown filter");
    }
};

#endif  // GRAPHPROCEXAMPLES_FILTER_FACTORY_H