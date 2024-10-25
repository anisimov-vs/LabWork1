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
#include <thread>

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
void Bitmap::load(std::string fileName) {
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        exit(1);
    }

    unsigned char fileType[2];
    file.read(reinterpret_cast<char*>(fileType), 2);

    if ((fileType[0] != 'B') || (fileType[1] != 'M')) {
        std::cerr << "Error: File is not a BMP file" << std::endl;
        exit(1);
    }

    bmpFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));

    bmpFileDibInfo dibInfo;
    file.read(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    isGrayscale = true;
    // Read palette if 8-bit
    if (dibInfo.bitsPerPixel == 8) {
        palette.resize(256, std::vector<uint8_t>(3));
        
        for (int i = 0; i < 256; i++) {
            file.read(reinterpret_cast<char*>(palette[i].data()), 3);
            file.ignore(1);

            if (palette[i][0] != palette[i][1] || palette[i][1] != palette[i][2] || palette[i][0] != palette[i][2]) {
                isGrayscale = false;
            }
        }
    }

    file.seekg(header.bmpOffset, std::ios::beg);

    // If it's a 1-bit monochrome image
    if (dibInfo.bitsPerPixel == 1) {
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

        for (int x = 0; x < dibInfo.height; x++) {
            std::vector<Pixel> pixelRow;

            for (int y = 0; y < dibInfo.width; y++) {
                uint8_t index = file.get();

                uint8_t red = palette[index][2];
                uint8_t green = palette[index][1];
                uint8_t blue = palette[index][0];

                Pixel pixel(red, green, blue);

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

                uint8_t blue = (pixelValue & 0x001F) * (255.0 / 31);
                uint8_t green = ((pixelValue & 0x07E0) >> 5) * (255.0 / 63); // Scale 6-bit to 8-bit
                uint8_t red = ((pixelValue & 0xF800) >> 11) * (255.0 / 31); // Scale 5-bit to 8-bit

                pixelRow.push_back(Pixel(red, green, blue));
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
                int blue = file.get();
                int green = file.get();
                int red = file.get();

                pixelRow.push_back(Pixel(red, green, blue));
            }

            file.seekg(dibInfo.width % 4, std::ios::cur);

            pixels.insert(pixels.begin(), pixelRow);
        }
    } else {
        std::cerr << "Error: Unsupported BMP format (only 1-bit, 8-bit grayscale, 16-bit, and 24-bit RGB are supported), you image is " << dibInfo.bitsPerPixel << " bits per pixel" << std::endl;
        exit(1);
    }
    std::cout << "Loaded " << getImageName(fileName) << " with " << dibInfo.width << "x" << dibInfo.height << " pixels and " << dibInfo.bitsPerPixel << " bits per pixel" << std::endl;

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
        header.bmpOffset += 256 * 4;  // Color palette (256 colors, 4 bytes each)
        header.fileSize = header.bmpOffset + (dibInfo.width * dibInfo.height + dibInfo.width % 4) * dibInfo.height;
    } else {
        header.fileSize = header.bmpOffset + (dibInfo.width * 3 + dibInfo.width % 4) * dibInfo.height;
    }

    file.write(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));
    file.write(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    // If 8-bit, write the color palette
    if (dibInfo.bitsPerPixel == 8) { 
        for (int i = 0; i < 256; i++) {
            file.put(palette[i][0]); // Blue
            file.put(palette[i][1]); // Green
            file.put(palette[i][2]); // Red
            file.put(0);
        }
    }

    // Write pixel data
    //for (int x = dibInfo.height - 1; x >= 0; x--) {
    for (int x = dibInfo.height - 1; x >= 0; x--) {
        const std::vector<Pixel> &rowPixels = pixels[x];
        for (int y = 0; y < dibInfo.width; y++) {
            const Pixel &pixel = rowPixels[y];

            if (dibInfo.bitsPerPixel == 8) {
                // Write 8-bit pixel
                file.put(pixel.blue);
            } else {
                // Write 24-bit pixel
                file.put(pixel.blue);
                file.put(pixel.green);
                file.put(pixel.red);
            }
        }

        // Write padding bytes
        for (int i = 0; i < dibInfo.width % 4; i++) {
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


void Bitmap::applyGaussianFilter(const std::vector<std::vector<float>>& kernel, int numThreads) {
    int32_t width = pixels.size();
    int32_t height = pixels[0].size();

    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }

    int stripHeight = height / numThreads;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        int startY = i * stripHeight;
        int endY = (i == numThreads - 1) ? height : (i + 1) * stripHeight;
        threads.emplace_back(&Bitmap::applyGaussianFilterThread, this, std::ref(kernel), startY, endY);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void Bitmap::applyGaussianFilterThread(const std::vector<std::vector<float>>& kernel, int startY, int endY) {
    int32_t width = pixels.size();
    int32_t height = pixels[0].size();
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

// Function to print help message
void printHelp() {
    std::cout << "Usage: main [options] [input_image] [output_dir] [num_threads]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i <input_image>     Specify the input image file  (default: image.bmp)" << std::endl;
    std::cout << "  -o <output_dir>      Specify the output directory  (default: output/)" << std::endl;
    std::cout << "  -n <num_threads>     Specify the number of threads (default: best possible)" << std::endl;
    std::cout << "  -h                   Display this help message" << std::endl;
    std::cout << "If input_image, output_dir or num_threads are not provided, default values are used." << std::endl;
}

void readArgs(int argc, char* argv[], std::string& inputImage, std::string& outputDir, int& numThreads) {
    numThreads = std::thread::hardware_concurrency();
    int opt;

    while ((opt = getopt(argc, argv, "i:o:n:h")) != -1) {
        switch (opt) {
        case 'i':
            if ((optarg) && (optarg[0] != '-')) {
                try {
                    inputImage = optarg; // Set number of threads from command-line argument
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Invalid argument for -i option." << std::endl;
                    printHelp();
                    exit(1);
                }
            } else {
                std::cerr << "Error: -i option requires an argument." << std::endl;
                printHelp();
                exit(1);
            }

            break;

        case 'o':
            if ((optarg) && (optarg[0] != '-')) {
                try {
                    outputDir = optarg; // Set number of threads from command-line argument
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Invalid argument for -o option." << std::endl;
                    printHelp();
                    exit(1);
                }
            } else {
                std::cerr << "Error: -o option requires an argument." << std::endl;
                printHelp();
                exit(1);
            }

            break;

        case 'h':
            printHelp();          // Display help message and exit
            exit(0);

        case 'n':
            if (optarg) {
                try {
                    numThreads = std::stoi(optarg); // Set number of threads from command-line argument
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Invalid argument for -n option." << std::endl;
                    printHelp();
                    return;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Argument for -n option is out of range." << std::endl;
                    printHelp();
                    return;
                }
            } else {
                std::cerr << "Error: -n option requires an argument." << std::endl;
                printHelp();
                exit(1);
            }

            break;
        default:
            printHelp();          // Display help message and exit with error

            return;
        }
    }

    std::cout << "Number of threads set to " << numThreads << std::endl;
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
}