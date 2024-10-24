// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int kernelSize = 5;  // Set the size of the Gaussian kernel
    float sigma = 1.0;   // Set the standard deviation for the Gaussian kernel
    std::vector<std::vector<float>> kernel = generateGaussianKernel(kernelSize, sigma);  // Generate the Gaussian kernel

    std::string inputImage = "image.bmp";
    std::string outputDir = "output/";
    int numThreads;

    readArgs(argc, argv, inputImage, outputDir, numThreads);

    std::string imageName = getImageName(inputImage);

    Bitmap image;
    image.load(inputImage); 

    // Rotate the image clockwise
    {
        Bitmap clockwise_image(image);  // Create a copy of the original image
        clockwise_image.rotate(true);   // Rotate the image clockwise
        clockwise_image.write(outputDir + "rotatedClockwise_" + imageName);  // Save the rotated image
    }

    std::cout << imageName << " rotated clockwise and saved to " << outputDir + "rotatedClockwise_" + imageName << std::endl;

    // Rotate the image counter-clockwise
    {
        Bitmap counterClockwise_image(image);  // Create a copy of the original image
        counterClockwise_image.rotate(false);  // Rotate the image counter-clockwise
        counterClockwise_image.write(outputDir + "rotatedCounterClockwise_" + imageName);  // Save the rotated image
    }

    std::cout << imageName << " rotated counter-clockwise and saved to " << outputDir + "rotatedCounterClockwise_" + imageName << std::endl;

    image.applyGaussianFilter(kernel, 0);  // Apply Gaussian filter to the original image

    std::cout << imageName << " Gaussian filtered" << std::endl;

    // Rotate the filtered image clockwise
    {
        Bitmap clockwise_image(image);
        clockwise_image.rotate(true);   // Rotate the image clockwise
        clockwise_image.write(outputDir + "filteredRotatedClockwise_" + imageName);  // Save the rotated image
    }

    std::cout << "Filtered " << imageName << " rotated clockwise and saved to " << outputDir + "filteredRotatedClockwise_" + imageName << std::endl;

    // Rotate the filtered image counter-clockwise
    {
        Bitmap counterClockwise_image(image);  // Create a copy of the original image
        counterClockwise_image.rotate(false);  // Rotate the image counter-clockwise
        counterClockwise_image.write(outputDir + "filteredRotatedCounterClockwise_" + imageName);  // Save the rotated image
    }

    std::cout << "Filtered " << imageName << " rotated counter-clockwise and saved to " << outputDir + "filteredRotatedCounterClockwise_" + imageName << std::endl;

    return 0;
}