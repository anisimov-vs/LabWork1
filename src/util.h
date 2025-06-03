// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef UTIL_h
#define UTIL_h

#include "bitmap.h"

#include <string>
#include <vector>

void printHelp();

struct Arguments {
  std::string inputImage = "image.bmp"; 
  std::string outputDir = "output/";
  int numThreads = 0;
  int kernelSize = 5;
  float sigma = 1.0;
};

Arguments readArgs(int argc, char* argv[]);
// Function to extract the image name from a file path
std::string getImageName(const std::string &imagePath);

// Function to generate a Gaussian kernel
std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);

bool rotateAndSave(Bitmap &image, const std::string &imageName, bool clockwise, const std::string &outputPath);

#endif