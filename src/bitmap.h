// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef BITMAP_h
#define BITMAP_h

#include <cstdint>
#include <string>
#include <vector>

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

// Class representing a single pixel with RGB values
class Pixel {
    public:
        uint8_t red, green, blue; // Red, Green, and Blue color components of the pixel

        Pixel() : red(0), green(0), blue(0) { } // Default constructor initializing pixel to black

        Pixel(float r, float g, float b) : red(r), green(g), blue(b) { } // Constructor to initialize pixel with specific values
};

// Class representing a Bitmap image
class Bitmap {
    public:
        std::vector<std::vector<Pixel>> pixels; // 2D vector storing the pixels of the image

        Bitmap() { } // Default constructor

        // Copy constructor
        Bitmap(const Bitmap& other) : pixels(other.pixels) { }

        // Constructor initializing a bitmap with given width and height
        Bitmap(int32_t width, int32_t height) : pixels(width, std::vector<Pixel>(height)) { }

        ~Bitmap() { pixels.clear(); } // Destructor clearing the pixel data

        // Method to load a bitmap from a file
        void load(std::string);

        // Method to write a bitmap to a file
        void write(std::string);

        // Method to rotate the bitmap image
        void rotate(bool clockwise);

        // Method to apply a Gaussian filter to the bitmap image
        void applyGaussianFilter(std::vector<std::vector<float>> kernel);
};

// Function to generate a Gaussian kernel
std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);

// Function to extract the image name from a file path
std::string getImageName(std::string &imagePath);

#endif
