// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include <iostream>


int main(int argc, char* argv[]) {
    int kernelSize = 5;  // Set the size of the Gaussian kernel
    float sigma = 1.0;   // Set the standard deviation for the Gaussian kernel
    std::vector<std::vector<float>> kernel = generateGaussianKernel(kernelSize, sigma);  // Generate the Gaussian kernel

    std::string *args = readArgs(argc, argv);
    std::string inputImage = args[0];
    std::string outputDir = args[1];

    std::string imageName = getImageName(inputImage);

    Bitmap image;
    image.load(inputImage); 
    image.write(outputDir + "original_" + imageName);

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