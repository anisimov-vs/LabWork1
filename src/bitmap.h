// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef BITMAP_h
#define BITMAP_h

#include <string>
#include <vector>

#pragma pack(push, 1)  // Start byte-aligned packing

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

#pragma pack(pop)  // Restore default packing

struct Pixel {
  uint8_t red, green, blue;
};

// Class representing a Bitmap image
class Bitmap {
  private:
    bool isGrayscale;

    std::vector<std::vector<Pixel>> pixels; // 2D vector storing the pixels of the image

  public:
    Bitmap() { } // Default constructor

    Bitmap(const std::string& filename) { load(filename); } // Constructor loading a bitmap from a file

    // Copy constructor
    Bitmap(const Bitmap& other) : isGrayscale(other.isGrayscale), pixels(other.pixels) { }

    ~Bitmap() { }

    // Method to load a bitmap from a file
    bool load(std::string);

    // Method to write a bitmap to a file
    bool write(std::string);

    // Method to rotate the bitmap image
    void rotate(bool clockwise);

    // Method to apply a Gaussian filter to the bitmap image
    void applyGaussianFilter(const std::vector<std::vector<float>>& kernel, int numThreads);
    void applyGaussianFilterThread(const std::vector<std::vector<float>>& kernel, int startY, int endY);
};

#endif
