
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>
#include "Kernels/Color.h"

template <typename Type, size_t numComponents = 1>
struct Image {

    size_t  width;
    size_t  height;
    Type*   pixels;

    Image() = default;

    Image(size_t width, size_t height, Type* pixels)
        : width(width), height(height), pixels(pixels) 
        { /* Empty */}

    Image(const Image&) = delete;

    size_t numPixels() const { return width * height; }
    size_t size() const { return numPixels() * sizeof(Type); }

    const Type* data() const { return pixels; }

    friend std::ofstream& operator << (std::ofstream& out, const Image& img) {
        const char* magic[] = { "", "P5", "", "P6" };
        out << magic[numComponents] << " " << img.width << " " << img.height << " " << 255 << "\n";
        out.write(reinterpret_cast<const char*>(img.pixels), img.size());
        return out;
    }
};

using GreyscaleImage = Image<Byte>;
using RGBImage       = Image<Color, 3>;

RGBImage readRGBImage(const char* filename) {
    FILE* file = fopen(filename, "rb");

    if (!file) {
        std::cerr << "Unable to open '" << filename << "' ... exiting\n";
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    auto mapSize = ftell(file);

    void* data = mmap(nullptr, mapSize, PROT_READ, MAP_PRIVATE, fileno(file), 0);

    std::istringstream in(reinterpret_cast<const char*>(data));
    Byte p6[4];
    in >> p6;  // read PPM magic number: "P6"

    size_t width, height;
    in >> width >> height;
    int maxColor;
    in >> maxColor;  // read color component maximum value

    auto headerSize = in.tellg() + std::streampos(1); // Include newline after header
    Color* pixels = reinterpret_cast<Color*>(data) + headerSize;

    return RGBImage(width, height, pixels);
}

#endif // __IMAGE_H__