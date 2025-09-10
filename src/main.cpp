// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "bitmap.h"
#include "util.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <future>

int main(int argc, char* argv[]) {
    Arguments args = readArgs(argc, argv);
    
    if (args.inputImage == "0") return 1;

    std::vector<std::vector<float>> kernel = generateGaussianKernel(args.kernelSize, args.sigma);  // Generate the Gaussian kernel

    const std::string imageName = getImageName(args.inputImage);

    Bitmap image;
    if (!image.load(args.inputImage)) return 1;

    // Launch initial rotations asynchronously to overlap with filtering
    std::cout << "Launching initial rotations..." << std::endl;
    auto fut_rot1 = std::async(std::launch::async, rotateAndSave, std::ref(image), imageName, true, args.outputDir + "rotatedClockwise_" + imageName);
    auto fut_rot2 = std::async(std::launch::async, rotateAndSave, std::ref(image), imageName, false, args.outputDir + "rotatedCounterClockwise_" + imageName);

    std::cout << "Filtering " << imageName << " with " << args.numThreads << " threads..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    image.applyGaussianFilter(kernel, args.numThreads);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Filter time: " << duration << " ms" << std::endl;

    // Wait for initial rotations to complete
    if (!fut_rot1.get()) return 1;
    if (!fut_rot2.get()) return 1;

    // Launch filtered rotations asynchronously
    std::cout << "Launching filtered rotations..." << std::endl;
    auto fut_filt_rot1 = std::async(std::launch::async, rotateAndSave, std::ref(image), imageName, true, args.outputDir + "filteredRotatedClockwise_" + imageName);
    auto fut_filt_rot2 = std::async(std::launch::async, rotateAndSave, std::ref(image), imageName, false, args.outputDir + "filteredRotatedCounterClockwise_" + imageName);
    if (!fut_filt_rot1.get()) return 1;
    if (!fut_filt_rot2.get()) return 1;
    
    return 0;
}