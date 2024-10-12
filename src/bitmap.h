#ifndef BITMAP_h
#define BITMAP_h

#include <cstdint>
#include <string>
#include <vector>


struct bmpFileHeader {
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bmpOffset;
};

struct bmpFileDibInfo {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors;
};


class Pixel {
    public:
        uint8_t red, green, blue;

        Pixel() : red(0), green(0), blue(0) { }

        Pixel(float r, float g, float b) : red(r), green(g), blue(b) { }

};

class Bitmap {
    public:
        std::vector <std::vector <Pixel> > pixels;

        Bitmap() { }
        Bitmap(const Bitmap& other) : pixels(other.pixels) { }
        Bitmap(int32_t width, int32_t height) : pixels(width, std::vector<Pixel>(height)) { }

        ~Bitmap() { pixels.clear(); }

        void load(std::string);
        void write(std::string);

        void rotate(bool clockwise);
        void applyGaussianFilter(std::vector<std::vector<float>> kernel);
};


std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);


#endif