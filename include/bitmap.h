// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef BITMAP_h
#define BITMAP_h

#include <string>
#include <vector>
#include <cstdint>

/**
 * @file bitmap.h
 * @brief Declarations for BMP file headers and the Bitmap class.
 */

#pragma pack(push, 1)  // Start byte-aligned packing

/**
 * @struct bmpFileHeader
 * @brief Represents the BMP file header.
 */
struct bmpFileHeader {
    /** File size in bytes. */
    uint32_t fileSize;
    /** Reserved; must be zero. */
    uint16_t reserved1;
    /** Reserved; must be zero. */
    uint16_t reserved2;
    /** Offset where the pixel data begins. */
    uint32_t bmpOffset;
};

/**
 * @struct bmpFileDibInfo
 * @brief Represents the BMP DIB header (BITMAPINFOHEADER).
 */
struct bmpFileDibInfo {
    /** Size of this header in bytes. */
    uint32_t headerSize;
    /** Image width in pixels. */
    int32_t width;
    /** Image height in pixels. */
    int32_t height;
    /** Number of color planes (must be 1). */
    uint16_t planes;
    /** Bits per pixel (e.g., 24 for RGB). */
    uint16_t bitsPerPixel;
    /** Compression method being used. */
    uint32_t compression;
    /** Size of the raw bitmap data (including padding). */
    uint32_t imageSize;
    /** Horizontal resolution (pixels per meter). */
    int32_t xPixelsPerMeter;
    /** Vertical resolution (pixels per meter). */
    int32_t yPixelsPerMeter;
    /** Number of colors in the palette. */
    uint32_t colorsUsed;
    /** Number of important colors. */
    uint32_t importantColors;
};

#pragma pack(pop)  // Restore default packing

/**
 * @struct Pixel
 * @brief Represents an RGB pixel with 8-bit channels.
 */
struct Pixel {
    uint8_t red, green, blue;
};

/**
 * @class Bitmap
 * @brief Encapsulates BMP image data and provides basic image operations.
 */
class Bitmap {
  private:
    bool isGrayscale;

    std::vector<std::vector<Pixel>> pixels; ///< 2D pixel storage

  public:
    /** @brief Default constructor. */
    Bitmap() { } // Default constructor

    /**
     * @brief Constructs and loads a BMP from file.
     * @param filename Path to the BMP file.
     */
    Bitmap(const std::string& filename) { load(filename); } // Constructor

    /** @brief Copy constructor. */
    Bitmap(const Bitmap& other) : isGrayscale(other.isGrayscale), pixels(other.pixels) { }

    /** @brief Destructor. */
    ~Bitmap() { }

    /**
     * @brief Loads a BMP image from a file.
     * @param filename Path to the BMP file.
     * @return True on success, false on failure.
     */
    bool load(const std::string &filename) const;

    /**
     * @brief Writes the BMP image to a file.
     * @param filename Output file path.
     * @return True on success, false on failure.
     */
    bool write(const std::string &filename) const;

    /**
     * @brief Rotates the image 90 degrees.
     * @param clockwise True for clockwise, false for counter-clockwise.
     */
    void rotate(bool clockwise);

    /**
     * @brief Applies a Gaussian filter to the image.
     * @param kernel Gaussian kernel matrix.
     * @param numThreads Number of threads to use.
     */
    void applyGaussianFilter(const std::vector<std::vector<float>>& kernel, int numThreads);

    /**
     * @internal
     * @brief Worker thread for the Gaussian filter.
     * @param kernel Gaussian kernel matrix.
     * @param startY Starting row index.
     * @param endY Ending row index (exclusive).
     */
    void applyGaussianFilterThread(const std::vector<std::vector<float>>& kernel, int startY, int endY);

    const std::vector<std::vector<Pixel>>& getPixels() const { return pixels; }
};

#endif
