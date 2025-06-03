// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#ifndef UTIL_h
#define UTIL_h

#include "bitmap.h"

#include <string>
#include <vector>

/**
 * @file util.h
 * @brief Utility functions for argument parsing, filename handling, and image rotation tasks.
 */

/** @brief Prints usage information to stdout. */
void printHelp();

/**
 * @struct Arguments
 * @brief Stores command-line argument values for the application.
 */
struct Arguments {
  /** Input image file path. */
  std::string inputImage = "image.bmp"; 
  /** Output directory path (must end with '/'). */
  std::string outputDir = "output/";
  /** Number of threads to use for filtering. */
  int numThreads = 0;
  /** Size of the Gaussian kernel (odd value). */
  int kernelSize = 5;
  /** Sigma value for the Gaussian kernel. */
  float sigma = 1.0;
};

/**
 * @brief Parses command-line arguments.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Filled Arguments struct, or default-initialized on error.
 */
Arguments readArgs(int argc, char* argv[]);

/**
 * @brief Extracts the filename from a full path.
 * @param imagePath Full image file path.
 * @return The filename component without directory prefixes.
 */
std::string getImageName(const std::string &imagePath);

/**
 * @brief Generates a 2D Gaussian kernel.
 * @param size Kernel dimension (must be odd).
 * @param sigma Standard deviation for Gaussian distribution.
 * @return A 2D vector representing the normalized kernel.
 */
std::vector<std::vector<float>> generateGaussianKernel(int size, float sigma);

/**
 * @brief Rotates an image and saves the result to disk.
 * @param image Bitmap instance to rotate.
 * @param imageName Base name for logging output messages.
 * @param clockwise True for clockwise rotation, false for counter-clockwise.
 * @param outputPath File path to save the rotated image.
 * @return True on success, false on failure.
 */
bool rotateAndSave(Bitmap &image, const std::string &imageName, bool clockwise, const std::string &outputPath);

#endif