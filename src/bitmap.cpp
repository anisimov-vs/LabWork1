// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>

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

    file.seekg(header.bmpOffset, std::ios::beg);

    // Read pixel data from the file
    for (int x = 0; x < dibInfo.height; x++) {
        std::vector <Pixel> pixelRow;

        for (int y = 0; y < dibInfo.width; y++) {
            int blue = file.get();
            int green = file.get();
            int red = file.get();
            
            pixelRow.push_back(Pixel(red, green, blue));
        }

        // Handle padding bytes
        file.seekg(dibInfo.width % 4, std::ios::cur);

        pixels.insert(pixels.begin(), pixelRow);
    }

    file.close();
}

// Write the BMP image to a file
void Bitmap::write(std::string fileName) {
    std::ofstream file(fileName, std::ios::binary);
    
    file.write("BM", 2);

    bmpFileHeader header = { 0 };

    header.bmpOffset = 2 + sizeof(bmpFileHeader) + sizeof(bmpFileDibInfo);
    header.fileSize = header.bmpOffset + (pixels.size() * 3 + pixels[0].size() % 4) * pixels.size();
    
    file.write(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));
    
    bmpFileDibInfo dibInfo = { 0 };
    
    dibInfo.headerSize = sizeof(bmpFileDibInfo);
    dibInfo.width = pixels[0].size();
    dibInfo.height = pixels.size();
    dibInfo.planes = 1;
    dibInfo.bitsPerPixel = 24;
    dibInfo.compression = 0;
    dibInfo.imageSize = dibInfo.width * dibInfo.height * 3;
    dibInfo.xPixelsPerMeter = 2835;
    dibInfo.yPixelsPerMeter = 2835;
    dibInfo.colorsUsed = 0;
    dibInfo.importantColors = 0;
    
    file.write(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    // Write pixel data to the file
    for (int x = pixels.size() - 1; x >= 0; x--) {
        const std::vector <Pixel> & rowPixels = pixels[x];
        
        for (int y = 0; y < pixels[x].size(); y++) {
            const Pixel& pixel = rowPixels[y];
            
            file.put((unsigned char)(pixel.blue));
            file.put((unsigned char)(pixel.green));
            file.put((unsigned char)(pixel.red));
        }

        // Write padding bytes
        for (int i = 0; i < rowPixels.size() % 4; i++) {
            file.put(0);
        }
    }

    file.close();
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
            pixels[x][y].green = uint8_t(green);
        }
    }
}