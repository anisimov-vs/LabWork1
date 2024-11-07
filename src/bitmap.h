// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef BITMAP_h
#define BITMAP_h

#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <thread>

// Structure representing the BMP file header
struct bmpFileHeader {
  uint32_t fileSize;       // Size of the BMP file in bytes
  uint16_t reserved1;      // Reserved; actual value depends on the application that creates the image
  uint16_t reserved2;      // Reserved; actual value depends on the application that creates the image
  uint32_t bmpOffset;      // Offset where the pixel array (bitmap data) can be found
};

// Structure containing DIB header information of a BMP file
struct bmpFileDibInfo {
  uint32_t headerSize;         // Size of this header
  int32_t width;               // Bitmap width in pixels
  int32_t height;              // Bitmap height in pixels
  uint16_t planes;             // Number of color planes being used
  uint16_t bitsPerPixel;       // Number of bits per pixel
  uint32_t compression;        // Compression method being used
  uint32_t imageSize;          // Size of the raw bitmap data
  int32_t xPixelsPerMeter;     // Horizontal resolution of the image (pixel per meter)
  int32_t yPixelsPerMeter;     // Vertical resolution of the image (pixel per meter)
  uint32_t colorsUsed;         // Number of colors in the color palette
  uint32_t importantColors;    // Number of important colors used
};

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

    Bitmap(std::string& filename) { load(filename); } // Constructor loading a bitmap from a file

    // Copy constructor
    Bitmap(const Bitmap& other) : isGrayscale(other.isGrayscale), pixels(other.pixels) { }

    ~Bitmap() { }

    // Method to load a bitmap from a file
    void load(std::string);

    // Method to write a bitmap to a file
    void write(std::string);

    // Method to rotate the bitmap image
    void rotate(bool clockwise);

    // Method to apply a Gaussian filter to the bitmap image
    void applyGaussianFilter(const std::vector<std::vector<float>>& kernel, int numThreads);
    void applyGaussianFilterThread(const std::vector<std::vector<float>>& kernel, int startY, int endY);
};

#endif
