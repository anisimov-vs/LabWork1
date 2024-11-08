// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include "util.h"

int main(int argc, char* argv[]) {
    Arguments args = readArgs(argc, argv);
    std::cout << "tfwnauyo" << std::endl;
    if (args.inputImage == "0") return 0;

    std::vector<std::vector<float>> kernel = generateGaussianKernel(args.kernelSize, args.sigma);  // Generate the Gaussian kernel

    std::string imageName = getImageName(args.inputImage);

    Bitmap image;
    if (!image.load(args.inputImage)) return 0;

    if (!rotateAndSave(image, imageName, true, args.outputDir + "rotatedClockwise_" + imageName)) return 0; // Rotate the image clockwise and save the result
    
    if (!rotateAndSave(image, imageName, false, args.outputDir + "rotatedCounterClockwise_" + imageName)) return 0; // Rotate the image counter-clockwise and save the result
    
    image.applyGaussianFilter(kernel, args.numThreads);  // Apply Gaussian filter to the original image
    std::cout << imageName << " Gaussian filtered" << std::endl;

    if (!rotateAndSave(image, imageName, true, args.outputDir + "filteredRotatedClockwise_" + imageName)) return 0; // Rotate the filtered image clockwise and save the result
    
    if (!rotateAndSave(image, imageName, false, args.outputDir + "filteredRotatedCounterClockwise_" + imageName)) return 0; // Rotate the filtered image counter-clockwise and save the result
    
    return 1;
}