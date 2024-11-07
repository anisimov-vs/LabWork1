// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include "util.h"

int main(int argc, char* argv[]) {
    Arguments args = readArgs(argc, argv);

    std::vector<std::vector<float>> kernel = generateGaussianKernel(args.kernelSize, args.sigma);  // Generate the Gaussian kernel

    std::string imageName = getImageName(args.inputImage);

    Bitmap image(args.inputImage);

    rotateAndSave(image, imageName, true, args.outputDir + "rotatedClockwise_" + imageName); // Rotate the image clockwise and save the result
    
    rotateAndSave(image, imageName, false, args.outputDir + "rotatedCounterClockwise_" + imageName); // Rotate the image counter-clockwise and save the result
    
    image.applyGaussianFilter(kernel, args.numThreads);  // Apply Gaussian filter to the original image
    std::cout << imageName << " Gaussian filtered" << std::endl;

    rotateAndSave(image, imageName, true, args.outputDir + "filteredRotatedClockwise_" + imageName); // Rotate the filtered image clockwise and save the result
    
    rotateAndSave(image, imageName, false, args.outputDir + "filteredRotatedCounterClockwise_" + imageName); // Rotate the filtered image counter-clockwise and save the result
    
    return 0;
}