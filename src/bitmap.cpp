// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <unistd.h>

// Function to generate a Gaussian kernel
std::vector<std::vector<float> > generateGaussianKernel(int size, float sigma) {
    std::vector<std::vector<float> > kernel(size, std::vector<float>(size));
    float sum = 0.0;
    int halfSize = size / 2;

    // Calculate the Gaussian values for each position in the kernel
    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            float exponent = -(x * x + y * y) / (2 * sigma * sigma);
            float value = exp(exponent) / (2 * M_PI * sigma * sigma);

            kernel[x + halfSize][y + halfSize] = value;
            sum += value;
        }
    }

    // Normalize the kernel to ensure the sum of all values is 1
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

// Extract the image name from a full path
std::string getImageName(std::string &imagePath) {
    std::string token;
    std::istringstream tokenStream(imagePath);
    std::string imageName;

    while (std::getline(tokenStream, token, '/')) {
        imageName = token;
    }
    
    return imageName;
}

// Load a BMP image from a file
void Bitmap::load(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    unsigned char fileType[2];
    file.read(reinterpret_cast<char*>(fileType), 2);
    
    if ((fileType[0] != 'B') && (fileType[1] != 'M')) {
        std::cerr << "Error: File is not a BMP file" << std::endl;
        exit(1);
    }

    bmpFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));

    bmpFileDibInfo dibInfo;
    file.read(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    // Check if the DIB header size indicates a V3 or later header
    //if (dibInfo.headerSize >= 56) {
        std::cout << dibInfo.headerSize << " " << dibInfo.bitsPerPixel << std::endl;
        // Read the color masks if present
        uint32_t redMask = 0x00FF0000;
        uint32_t greenMask = 0x0000FF00;
        uint32_t blueMask = 0x000000FF;
        uint32_t alphaMask = 0xFF000000;

        if (dibInfo.headerSize == 56 || dibInfo.headerSize == 108 || dibInfo.headerSize == 124) {
            file.read(reinterpret_cast<char*>(&redMask), sizeof(redMask));
            file.read(reinterpret_cast<char*>(&greenMask), sizeof(greenMask));
            file.read(reinterpret_cast<char*>(&blueMask), sizeof(blueMask));
            if (dibInfo.headerSize >= 108) {
                file.read(reinterpret_cast<char*>(&alphaMask), sizeof(alphaMask));
            }
        }

        // Seek to the pixel data start position
        file.seekg(header.bmpOffset, std::ios::beg);

        // Handle 16-bit BMP with custom masks
        if (dibInfo.bitsPerPixel == 16) {
            isGrayscale = false;
            for (int x = 0; x < dibInfo.height; x++) {
                std::vector<Pixel> pixelRow;
                for (int y = 0; y < dibInfo.width; y++) {
                    uint16_t pixelValue = (file.get() | (file.get() << 8));
                    uint8_t blue = ((pixelValue & blueMask) >> __builtin_ctz(blueMask)) * (255.0 / ((1 << __builtin_popcount(blueMask)) - 1));
                    uint8_t green = ((pixelValue & greenMask) >> __builtin_ctz(greenMask)) * (255.0 / ((1 << __builtin_popcount(greenMask)) - 1));
                    uint8_t red = ((pixelValue & redMask) >> __builtin_ctz(redMask)) * (255.0 / ((1 << __builtin_popcount(redMask)) - 1));
                    pixelRow.push_back(Pixel(red, green, blue));
                }
                file.seekg(dibInfo.width % 4, std::ios::cur);
                pixels.insert(pixels.begin(), pixelRow);
            }
        }

    //} else {
        // Seek to the pixel data start position
        //file.seekg(header.bmpOffset, std::ios::beg);

        // If it's a 1-bit monochrome image
        else if (dibInfo.bitsPerPixel == 1) {
            isGrayscale = true;

            // Read pixel data (each byte contains 8 pixels, packed as bits)
            for (int x = 0; x < dibInfo.height; x++) {
                std::vector<Pixel> pixelRow;
                for (int y = 0; y < (dibInfo.width + 7) / 8; y++) {
                    uint8_t byte = file.get();  // Read one byte (8 pixels)
                    for (int bit = 0; bit < 8 && y * 8 + bit < dibInfo.width; bit++) {

                        uint8_t colorIndex = (byte >> (7 - bit)) & 0x01;

                        Pixel pixel(0, 0, 0);
                        if (colorIndex == 0) {
                            pixel.blue = 255;
                            pixel.green = 255;
                            pixel.red = 255;
                        }
                        pixelRow.push_back(pixel);
                    }
                }
                // Handle padding bytes (each row must be a multiple of 4 bytes)
                file.seekg((4 - ((dibInfo.width + 7) / 8) % 4) % 4, std::ios::cur);

                pixels.insert(pixels.begin(), pixelRow);
            }
        } 
        // Handle 8-bit grayscale
        else if (dibInfo.bitsPerPixel == 8) {
            isGrayscale = true;
            std::vector<std::vector<uint8_t>> palette(256, std::vector<uint8_t>(4));

            for (int x = 0; x < dibInfo.height; x++) {
                std::vector<Pixel> pixelRow;
                for (int y = 0; y < dibInfo.width; y++) {
                    uint8_t grayValue = file.get();
                    Pixel pixel(grayValue, grayValue, grayValue);
                    pixelRow.push_back(pixel);
                }
                file.seekg(dibInfo.width % 4, std::ios::cur);
                pixels.insert(pixels.begin(), pixelRow);
            }
        }
        // Handle 24-bit RGB
        else if (dibInfo.bitsPerPixel == 24) {
            isGrayscale = false;
            for (int x = 0; x < dibInfo.height; x++) {
                std::vector<Pixel> pixelRow;
                for (int y = 0; y < dibInfo.width; y++) {
                    int blue = file.get();
                    int green = file.get();
                    int red = file.get();
                    pixelRow.push_back(Pixel(red, green, blue));
                }
                file.seekg(dibInfo.width % 4, std::ios::cur);
                pixels.insert(pixels.begin(), pixelRow);
            }
        } else {
            std::cerr << "Error: Unsupported BMP format (only 1-bit, 8-bit grayscale, 16-bit, and 24-bit RGB are supported)" << std::endl;
            exit(1);
        }
    //}

    file.close();
}

// Write the BMP image to a file
void Bitmap::write(std::string fileName) {
    std::ofstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    file.write("BM", 2);

    bmpFileHeader header = {0};
    header.bmpOffset = 2 + sizeof(bmpFileHeader) + sizeof(bmpFileDibInfo);

    if (dibInfo.bitsPerPixel == 8) {
        header.bmpOffset += 256 * 4;  // Color palette (256 colors, 4 bytes each)
        header.fileSize = header.bmpOffset + (dibInfo.width * dibInfo.height + dibInfo.width % 4) * dibInfo.height;
    } else {
        header.fileSize = header.bmpOffset + (dibInfo.width * 3 + dibInfo.width % 4) * dibInfo.height;
    }

    file.write(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));

    bmpFileDibInfo dibInfo = {0};
    dibInfo.headerSize = sizeof(bmpFileDibInfo);
    dibInfo.width = pixels[0].size();
    dibInfo.height = pixels.size();
    dibInfo.planes = 1;
    
    if (dibInfo.bitsPerPixel == 8) {
        dibInfo.bitsPerPixel = 8;
        dibInfo.compression = 0;
        dibInfo.imageSize = dibInfo.width * dibInfo.height;
        dibInfo.colorsUsed = 256;
    } else {
        dibInfo.bitsPerPixel = 24;
        dibInfo.imageSize = dibInfo.width * dibInfo.height * 3;
        dibInfo.colorsUsed = 0;
    }
    dibInfo.xPixelsPerMeter = 2835;
    dibInfo.yPixelsPerMeter = 2835;
    dibInfo.importantColors = 0;

    file.write(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    // If 8-bit, write the color palette
    if (dibInfo.bitsPerPixel == 8) {
        for (int i = 0; i < 256; i++) {
            file.put(palette[i][0]); // Blue
            file.put(palette[i][1]); // Green
            file.put(palette[i][2]); // Red
            file.put(0); // Reserved
        }
    }

    // Write pixel data
    for (int x = dibInfo.height - 1; x >= 0; x--) {
        const std::vector<Pixel> &rowPixels = pixels[x];
        for (int y = 0; y < dibInfo.width; y++) {
            const Pixel &pixel = rowPixels[y];
            if (dibInfo.bitsPerPixel == 8) {
                // Write 8-bit pixel index
                uint8_t index = findClosestPaletteIndex(pixel);
                file.put(index);
            } else {
                // Write 24-bit color pixel (BGR)
                file.put((unsigned char)(pixel.blue));
                file.put((unsigned char)(pixel.green));
                file.put((unsigned char)(pixel.red));
            }
        }

        // Write padding bytes
        for (int i = 0; i < dibInfo.width % 4; i++) {
            file.put(0);
        }
    }

    file.close();
}

// Helper function to find the closest palette index for a given pixel
uint8_t Bitmap::findClosestPaletteIndex(const Pixel &pixel) {
    uint8_t closestIndex = 0;
    int closestDistance = INT_MAX;
    for (int i = 0; i < 256; i++) {
        int distance = (pixel.red - palette[i][2]) * (pixel.red - palette[i][2]) +
                       (pixel.green - palette[i][1]) * (pixel.green - palette[i][1]) +
                       (pixel.blue - palette[i][0]) * (pixel.blue - palette[i][0]);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestIndex = i;
        }
    }
    return closestIndex;
}


// Rotate the image 90 degrees clockwise or counterclockwise
void Bitmap::rotate(bool clockwise) {
    int32_t width = pixels.size();
    int32_t heigth = pixels[0].size();

    std::vector <std::vector <Pixel> > rotatedPixels(heigth, std::vector <Pixel>(width));

    for (int32_t x = 0; x < width; x++) {
        for (int32_t y = 0; y < heigth; y++) {
            if (clockwise) {
                rotatedPixels[y][width - x - 1] = pixels[x][y];
            } else {
                rotatedPixels[heigth - y - 1][x] = pixels[x][y];
            }
        }
    }
    
    pixels = rotatedPixels;
}

// Apply a Gaussian filter to the image
void Bitmap::applyGaussianFilter(std::vector<std::vector<float> > kernel) {
    int32_t width = pixels.size();
    int32_t heigth = pixels[0].size();

    int halfKernelSize = kernel.size() / 2;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < heigth; y++) {
            float red = 0.0, green = 0.0, blue = 0.0;

            // Convolve the kernel with the image
            for (int i = -halfKernelSize; i <= halfKernelSize; i++) {
                for (int j = -halfKernelSize; j <= halfKernelSize; j++) {
                    int nx = x + i;
                    int ny = y + j;

                    if (nx < 0) nx = 0;
                    if (nx >= width) nx = width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= heigth) ny = heigth -1;

                    red += pixels[nx][ny].red * kernel[i + halfKernelSize][j + halfKernelSize];
                    green += pixels[nx][ny].green * kernel[i + halfKernelSize][j + halfKernelSize];
                    blue += pixels[nx][ny].blue * kernel[i + halfKernelSize][j + halfKernelSize];
                }
            }

            pixels[x][y].red = uint8_t(red);
            pixels[x][y].green = uint8_t(green);
            pixels[x][y].blue = uint8_t(blue);
        }
    }
}

// Function to print help message
void printHelp() {
    std::cout << "Usage: main [options] [input_image] [output_dir]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i <input_image>    Specify the input image file (default: image.bmp)" << std::endl;
    std::cout << "  -o <output_dir>     Specify the output directory (default: output/)" << std::endl;
    std::cout << "  -h                  Display this help message" << std::endl;
    std::cout << "If input_image and output_dir are not provided, default values are used." << std::endl;
    std::cout << "Default input_image: image.bmp" << std::endl;
    std::cout << "Default output_dir: output/" << std::endl;
}

std::string *readArgs(int argc, char* argv[]) {
    std::string inputImage = "image.bmp";
    std::string outputDir = "output/";

    int opt;
    // Parse command-line options
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                inputImage = optarg;  // Set input image file from command-line argument
                break;
            case 'o':
                outputDir = optarg;   // Set output directory from command-line argument
                break;
            case 'h':
                printHelp();          // Display help message and exit
                return nullptr;
            default:
                printHelp();          // Display help message and exit with error
                return nullptr;
        }
    }

    // Check for positional arguments (input image and output directory)
    if (optind < argc) {
        inputImage = argv[optind++];  // Set input image file from positional argument
    }
    if (optind < argc) {
        outputDir = argv[optind++];   // Set output directory from positional argument
    }

    // Ensure the output directory ends with a '/'
    if (outputDir.back() != '/') {
        outputDir += '/';
    }

    // Allocate a dynamic array of two strings to store the results
    std::string *result = new std::string[2];
    result[0] = inputImage;
    result[1] = outputDir;

    return result;  // Return the pointer to the array
}