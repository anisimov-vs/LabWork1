// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <iostream>
#include <unistd.h>

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


int main(int argc, char* argv[]) {
    int kernelSize = 5;  // Set the size of the Gaussian kernel
    float sigma = 1.0;   // Set the standard deviation for the Gaussian kernel
    std::vector<std::vector<float>> kernel = generateGaussianKernel(kernelSize, sigma);  // Generate the Gaussian kernel

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
                return 0;
            default:
                printHelp();          // Display help message and exit with error
                return 1;
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

    std::string imageName = getImageName(inputImage);

    Bitmap image;
    image.load(inputImage); 

    // Rotate the image clockwise and apply Gaussian filter
    {
        Bitmap clockwise_image(image);  // Create a copy of the original image
        clockwise_image.rotate(true);   // Rotate the image clockwise
        clockwise_image.write(outputDir + "rotatedClockwise_" + imageName);  // Save the rotated image
        clockwise_image.applyGaussianFilter(kernel);  // Apply Gaussian filter to the rotated image
        clockwise_image.write(outputDir + "filteredRotatedClockwise_" + imageName);  // Save the filtered image
    }

    std::cout << imageName << " rotated clockwise and filtered" << std::endl;

    // Rotate the image counter-clockwise and apply Gaussian filter
    {
        Bitmap counterClockwise_image(image);  // Create a copy of the original image
        counterClockwise_image.rotate(false);  // Rotate the image counter-clockwise
        counterClockwise_image.write(outputDir + "rotatedCounterClockwise_" + imageName);  // Save the rotated image
        counterClockwise_image.applyGaussianFilter(kernel);  // Apply Gaussian filter to the rotated image
        counterClockwise_image.write(outputDir + "filteredRotatedCounterClockwise_" + imageName);  // Save the filtered image
    }

    std::cout << imageName << " rotated counterclockwise and filtered" << std::endl;

    return 0;
}