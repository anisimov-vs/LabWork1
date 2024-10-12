// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <iostream>
#include <unistd.h>

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

int main(int argc, char* argv[]) {
    int kernelSize = 5;
    float sigma = 1.0;
    std::vector<std::vector<float>> kernel = generateGaussianKernel(kernelSize, sigma);

    std::string inputImage = "image.bmp";
    std::string outputDir = "output/";

    int opt;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                inputImage = optarg;
                break;
            case 'o':
                outputDir = optarg;
                break;
            case 'h':
                printHelp();
                return 0;
            default:
                printHelp();
                return 1;
        }
    }
    // Check for positional arguments
    if (optind < argc) {
        inputImage = argv[optind++];
    }
    if (optind < argc) {
        outputDir = argv[optind++];
    }

    if (outputDir.back() != '/') {
        outputDir += '/';
    }

    Bitmap image;
    image.load(inputImage);
    printf("Image loaded\n");

    {
        Bitmap clockwise_image(image);
        clockwise_image.rotate(true);
        printf("Image rotated clockwise\n");
        clockwise_image.write(outputDir + "rotatedClockwise_image.bmp");
        printf("Image written\n");
        clockwise_image.applyGaussianFilter(kernel);
        printf("Image filtered\n");
        clockwise_image.write(outputDir + "filteredRotatedClockwise_image.bmp");
        printf("Image written\n");
    }
    printf("Image rotated clockwise\n");

    {
        Bitmap counterClockwise_image(image);
        counterClockwise_image.rotate(false);
        printf("Image rotated counter clockwise\n");
        counterClockwise_image.write(outputDir + "rotatedCounterClockwise_image.bmp");
        printf("Image written\n");
        counterClockwise_image.applyGaussianFilter(kernel);
        printf("Image filtered\n");
        counterClockwise_image.write(outputDir + "filteredRotatedCounterClockwise_image.bmp");
        printf("Image written\n");
    }
    printf("Image rotated counterclockwise\n");

    return 0;
}