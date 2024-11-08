// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include "util.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <thread>

// Load a BMP image from a file
bool Bitmap::load(std::string fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return 0;
    }

    unsigned char fileType[2];
    file.read(reinterpret_cast<char*>(fileType), 2);

    if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
        std::cerr << "Error: File is not a BMP file" << std::endl;
        return 0;
    }

    bmpFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));

    bmpFileDibInfo dibInfo;
    file.read(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    std::cout << "Memory required to load " << fileName << ": " << dibInfo.height * dibInfo.width * 3 / 1024 << "KB" << std::endl;
    
    std::vector<std::vector<uint8_t>> palette;
    
    // Read palette if 8-bit
    if (dibInfo.bitsPerPixel == 8) {
        isGrayscale = true;
        
        dibInfo.colorsUsed = dibInfo.colorsUsed + 256*(!(bool)dibInfo.colorsUsed);
        
        palette.resize(dibInfo.colorsUsed, std::vector<uint8_t>(4));

        for (int i = 0; i < dibInfo.colorsUsed; i++) {
            file.read(reinterpret_cast<char*>(palette[i].data()), 4);

            if (palette[i][0] != palette[i][1] || palette[i][1] != palette[i][2] || palette[i][0] != palette[i][2]) {
                isGrayscale = false;
            }
        }
    }

    file.seekg(header.bmpOffset, std::ios::beg);

    // Handle 1-bit monochrome BMP
    if (dibInfo.bitsPerPixel == 1) {
        isGrayscale = true;
        // Read pixel data (each byte contains 8 pixels, packed as bits)
        for (int x = 0; x < dibInfo.height; x++) {
            std::vector<Pixel> pixelRow;
            for (int y = 0; y < (dibInfo.width + 7) / 8; y++) {
                uint8_t byte = file.get();  // Read one byte (8 pixels)

                for (int bit = 0; bit < 8 && y * 8 + bit < dibInfo.width; bit++) {
                    uint8_t colorIndex = (byte >> (7 - bit)) & 0x01;

                    Pixel pixel;

                    pixel.blue = 255 * !colorIndex;
                    pixel.green = 255 * !colorIndex;
                    pixel.red = 255 * !colorIndex;

                    pixelRow.push_back(pixel);
                }
            }
            // Handle padding bytes (each row must be a multiple of 4 bytes)
            file.seekg((4 - ((dibInfo.width + 7) / 8) % 4) % 4, std::ios::cur);

            pixels.insert(pixels.begin(), pixelRow);
        }
    }
    // Handle 8-bit BMP
    else if (dibInfo.bitsPerPixel == 8) {

        for (int x = 0; x < dibInfo.height; x++) {
            std::vector<Pixel> pixelRow;

            for (int y = 0; y < dibInfo.width; y++) {
                uint8_t index = file.get();

                Pixel pixel;

                pixel.red = palette[index][2];
                pixel.green = palette[index][1];
                pixel.blue = palette[index][0];

                pixelRow.push_back(pixel);
            }

            file.seekg(dibInfo.width % 4, std::ios::cur);

            pixels.insert(pixels.begin(), pixelRow);
        }
    }
    // Handle 16-bit BMP with custom masks
    else if (dibInfo.bitsPerPixel == 16) {
        isGrayscale = false;

        for (int x = 0; x < dibInfo.height; x++) {
            std::vector<Pixel> pixelRow;

            for (int y = 0; y < dibInfo.width; y++) {
                uint16_t pixelValue = (file.get() | (file.get() << 8));
                
                Pixel pixel;
                
                pixel.blue = (pixelValue & 0x001F) * (255.0 / 31);
                pixel.green = ((pixelValue & 0x07E0) >> 5) * (255.0 / 63); // Scale 6-bit to 8-bit
                pixel.red = ((pixelValue & 0xF800) >> 11) * (255.0 / 31); // Scale 5-bit to 8-bit

                pixelRow.push_back(pixel);
            }

            file.seekg(dibInfo.width % 4, std::ios::cur);

            pixels.insert(pixels.begin(), pixelRow);
        }
    }
    // Handle 24-bit RGB
    else if (dibInfo.bitsPerPixel == 24) {
        file.seekg(header.bmpOffset, std::ios::beg);
        isGrayscale = false;

        for (int x = 0; x < dibInfo.height; x++) {
            std::vector<Pixel> pixelRow;

            for (int y = 0; y < dibInfo.width; y++) {
                Pixel pixel;

                pixel.blue = file.get();
                pixel.green = file.get();
                pixel.red = file.get();    

                pixelRow.push_back(pixel);
            }

            file.seekg(dibInfo.width % 4, std::ios::cur);

            pixels.insert(pixels.begin(), pixelRow);
        }
    } else {
        std::cerr << "Error: Unsupported BMP format (only 1-bit, 8-bit grayscale, 16-bit, and 24-bit RGB are supported), your image is " << dibInfo.bitsPerPixel << " bits per pixel" << std::endl;
        return 0;
    }
    std::cout << "Loaded " << getImageName(fileName) << " with " << dibInfo.width << "x" << dibInfo.height << " pixels and " << dibInfo.bitsPerPixel << " bits per pixel" << std::endl;

    file.close();

    return 1;
}

// Write the BMP image to a file
bool Bitmap::write(std::string fileName) {
    std::ofstream file(fileName, std::ios::binary);

    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 0;
    }

    file.write("BM", 2);

    bmpFileHeader header;
    header.bmpOffset = 2 + sizeof(bmpFileHeader) + sizeof(bmpFileDibInfo);

    bmpFileDibInfo dibInfo = {0};
    dibInfo.headerSize = sizeof(bmpFileDibInfo);
    dibInfo.width = pixels[0].size();
    dibInfo.height = pixels.size();
    dibInfo.planes = 1;

    if (isGrayscale) {
        dibInfo.bitsPerPixel = 8;
        dibInfo.compression = 0;
        dibInfo.imageSize = dibInfo.width * dibInfo.height;
        dibInfo.colorsUsed = 256;
    } else {
        dibInfo.bitsPerPixel = 24;
        dibInfo.imageSize = dibInfo.width * dibInfo.height * 3;
        dibInfo.colorsUsed = 0;
    }

    dibInfo.xPixelsPerMeter = 2834;
    dibInfo.yPixelsPerMeter = 2834;
    dibInfo.importantColors = 0;

    if (dibInfo.bitsPerPixel == 8) {
        header.bmpOffset += 256 * 4;  // +Color palette (256 colors, 4 bytes each)
        header.fileSize = header.bmpOffset + (dibInfo.width * dibInfo.height + dibInfo.width % 4) * dibInfo.height;
    } else {
        header.fileSize = header.bmpOffset + (dibInfo.width * 3 + dibInfo.width % 4) * dibInfo.height;
    }

    file.write(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));
    file.write(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    // If 8-bit grayscale, write base color palette
    if (dibInfo.bitsPerPixel == 8) { 
        for (int i = 0; i < 256; i++) {
            file.put(i);
            file.put(i);
            file.put(i);
            file.put(0);
        }
    }

    int bytesPerPixel = dibInfo.bitsPerPixel / 8;
    int rowSize = dibInfo.width * bytesPerPixel;
    int paddingSize = (4 - (rowSize % 4)) % 4;

    // Preallocate a buffer large enough for one row plus padding
    std::vector<char> buffer(rowSize + paddingSize, 0); // Initialize padding bytes to 0

    for (int x = dibInfo.height - 1; x >= 0; x--) {
        const std::vector<Pixel> &rowPixels = pixels[x];
        
        // Fill the buffer with pixel data
        for (int y = 0; y < dibInfo.width; y++) {
            const Pixel &pixel = rowPixels[y];
            
            if (dibInfo.bitsPerPixel == 8) {
                buffer[y] = static_cast<char>(pixel.blue);
            } else if (dibInfo.bitsPerPixel == 24) {
                buffer[y * 3] = static_cast<char>(pixel.blue);
                buffer[y * 3 + 1] = static_cast<char>(pixel.green);
                buffer[y * 3 + 2] = static_cast<char>(pixel.red);
            }
        }

        file.write(buffer.data(), buffer.size());
    }

    file.close();

    return 1;
}

// Rotate the image 90 degrees clockwise or counterclockwise
void Bitmap::rotate(bool clockwise) {
    int width = pixels.size();
    int height = pixels[0].size();

    std::vector <std::vector <Pixel> > rotatedPixels(height, std::vector <Pixel>(width));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (clockwise) {
                rotatedPixels[y][width - x - 1] = pixels[x][y];
            } else {
                rotatedPixels[height - y - 1][x] = pixels[x][y];
            }
        }
    }

    pixels = rotatedPixels;
}


void Bitmap::applyGaussianFilter(const std::vector<std::vector<float>>& kernel, int numThreads) {
    int height = pixels[0].size();

    int stripHeight = height / numThreads;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        int startY = i * stripHeight;
        int endY = (i == numThreads - 1) ? height : (i + 1) * stripHeight;

        threads.emplace_back(&Bitmap::applyGaussianFilterThread, this, std::ref(kernel), startY, endY);
    }

    for (auto&& thread : threads) {
        thread.join();
    }
}

void Bitmap::applyGaussianFilterThread(const std::vector<std::vector<float>>& kernel, int startY, int endY) {
    int width = pixels.size();
    int height = pixels[0].size();
    int halfKernelSize = kernel.size() / 2;

    for (int x = 0; x < width; x++) {
        for (int y = startY; y < endY; y++) {
            float red = 0.0, green = 0.0, blue = 0.0;

            // Convolve the kernel with the image
            for (int i = -halfKernelSize; i <= halfKernelSize; i++) {
                for (int j = -halfKernelSize; j <= halfKernelSize; j++) {
                    int nx = x + i;
                    int ny = y + j;

                    if (nx < 0) nx = 0;
                    if (nx >= width) nx = width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= height) ny = height - 1;

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

