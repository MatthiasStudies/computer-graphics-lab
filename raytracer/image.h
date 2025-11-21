#ifndef IMAGE_H
#define IMAGE_H

#include "math.h"
#include <vector>
#include <fstream>
#include <string>

class Image {
    int width_;
    int height_;
    std::vector<Vector3df> pixels_; // rgb in [0,1]
public:
    Image(int w, int h) : width_(w), height_(h), pixels_(w*h, {0.0f,0.0f,0.0f}) {}

    int width() const { return width_; }
    int height() const { return height_; }

    void set_pixel(int x, int y, const Vector3df &color) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
        pixels_[y*width_ + x] = color;
    }

    const Vector3df &get_pixel(int x, int y) const {
        return pixels_[y*width_ + x];
    }

    void save_ppm(const std::string &filename) const {
        std::ofstream out(filename, std::ios::binary);
        out << "P6\n" << width_ << " " << height_ << "\n255\n";
        for (const auto &c : pixels_) {
            auto clamp = [](float v){ return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); };
            unsigned char r = static_cast<unsigned char>(255.0f * clamp(c.vector[0]));
            unsigned char g = static_cast<unsigned char>(255.0f * clamp(c.vector[1]));
            unsigned char b = static_cast<unsigned char>(255.0f * clamp(c.vector[2]));
            out.put(r); out.put(g); out.put(b);
        }
    }
};

#endif
