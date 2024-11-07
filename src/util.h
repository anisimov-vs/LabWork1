// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef UTIL_h
#define UTIL_h

#include "bitmap.h"

struct Arguments {
  std::string inputImage = "image.bmp"; 
  std::string outputDir = "output/";
  int numThreads = 0;
  int kernelSize = 5;
  float sigma = 1.0;
};

// Function to generate a Gaussian kernel
std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);

// Function to extract the image name from a file path
std::string getImageName(std::string &imagePath);

void printHelp();

Arguments readArgs(int argc, char* argv[]);

void rotateAndSave(Bitmap &image, std::string imageName, bool clockwise, std::string outputPath);

#endif