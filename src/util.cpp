// Anisimov Vasiliy st129629@student.spbu.ru
// Laboratory Work 1

#include "util.h"

// Function to print help message
void printHelp() {
    std::cout << "Usage: main [options] [input_image] [output_dir] [num_threads] [kernel_size] [sigma]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i <input_image>     Specify the input image file  (default: image.bmp)" << std::endl;
    std::cout << "  -o <output_dir>      Specify the output directory  (default: output/)" << std::endl;
    std::cout << "  -n <num_threads>     Specify the number of threads (default: best possible)" << std::endl;
    std::cout << "  -k <kernel_size>     Specify the kernel size (default: 5)" << std::endl;
    std::cout << "  -s <sigma>           Specify the sigma (default: 1.0)" << std::endl;
    std::cout << "  -h                   Display this help message" << std::endl;
    std::cout << "If input_image, output_dir, num_threads, kernel_size or sigma are not provided, default values are used." << std::endl;
}

Arguments readArgs(int argc, char* argv[]) {
    Arguments args;
    int opt;

    while ((opt = getopt(argc, argv, "i:o:n:k:s:h")) != -1) {
        switch (opt) {
            case 'i':
                if ((optarg) && (optarg[0] != '-')) {
                    try {
                        args.inputImage = optarg;
                    } catch (...) {
                        std::cerr << "Error: Invalid argument for -i option." << std::endl;
                        printHelp();
                        return Arguments{0};
                    }
                } else {
                    std::cerr << "Error: -i option requires an argument." << std::endl;
                    printHelp();
                    std::cout << "tfwat\n";
                    return Arguments{0};
                }
                break;

            case 'o':
                if ((optarg) && (optarg[0] != '-')) {
                    try {
                        args.outputDir = optarg;
                    } catch (...) {
                        std::cerr << "Error: Invalid argument for -o option." << std::endl;
                        printHelp();
                        return Arguments{0};
                    }
                } else {
                    std::cerr << "Error: -o option requires an argument." << std::endl;
                    printHelp();
                    return Arguments{0};
                }

                break;

            case 'n':
                if (optarg) {
                    try {
                        args.numThreads = std::stoi(optarg);
                    } catch (...) {
                        std::cerr << "Error: Invalid argument for -n option." << std::endl;
                        printHelp();
                        return Arguments{0};    
                    }
                } else {
                    std::cerr << "Error: -n option requires an argument." << std::endl;
                    printHelp();
                    return Arguments{0};
                }

                break;
            
            case 'k':
                if (optarg) {
                    try {
                        args.kernelSize = std::stoi(optarg) / 2 * 2 + 1;
                    } catch (...) {
                        std::cerr << "Error: Invalid argument for -k option." << std::endl;
                        printHelp();
                        return Arguments{0}; 
                    }
                } else {
                    std::cerr << "Error: -k option requires an argument." << std::endl;
                    printHelp();
                    return Arguments{0};
                }

                break;
            
            case 's':
                if (optarg) {
                    try {
                        args.sigma = std::stoi(optarg);
                    } catch (...) {
                        std::cerr << "Error: Invalid argument for -s option." << std::endl;
                        printHelp();
                        return Arguments{0};    
                    }
                } else {
                    std::cerr << "Error: -s option requires an argument." << std::endl;
                    printHelp();
                    return Arguments{0};
                }

                break;

            case 'h':
                printHelp();
                return Arguments{0};

            default:
                printHelp();
                return Arguments{0};
        }
    }

    // Check for positional arguments (input image and output directory)
    if (optind < argc) {
        args.inputImage = argv[optind++];
    }
    if (optind < argc) {
        args.outputDir = argv[optind++];
    }
    if (optind < argc) {
        args.numThreads = std::stoi(argv[optind++]);
    }
    if (optind < argc) {
        args.kernelSize = std::stoi(argv[optind++]);
    }
    if (optind < argc) {
        args.sigma = std::stoi(argv[optind++]);
    }
    if (optind < argc) {
        std::cerr << "Error: Too many arguments." << std::endl;
        printHelp();
        return Arguments{0};
    }

    if (args.numThreads <= 0) {
        args.numThreads = std::thread::hardware_concurrency();
    }

    std::cout << "Number of threads is set to " << args.numThreads << std::endl;

    // Ensure the output directory ends with a '/'
    if (args.outputDir.back() != '/') {
        args.outputDir += '/';
    }

    return args;
}

// Extract the image name from a full path
std::string getImageName(std::string &imagePath) {
    std::string token;
    std::istringstream tokenStream(imagePath);
    std::string imageName;

    while (std::getline(tokenStream, token, '/')) {
        imageName = token;
    }

    return imageName;
}

// Function to generate a Gaussian kernel
std::vector<std::vector<float> > generateGaussianKernel(int size, float sigma) {
    std::vector<std::vector<float> > kernel(size, std::vector<float>(size));

    float sum = 0.0;
    int halfSize = size / 2;

    // Calculate the Gaussian values for each position in the kernel
    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            float exponent = -(x * x + y * y) / (2 * sigma * sigma);
            float value = exp(exponent) / (2 * M_PI * sigma * sigma);

            kernel[x + halfSize][y + halfSize] = value;
            sum += value;
        }
    }

    // Normalize the kernel to ensure the sum of all values is 1
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

bool rotateAndSave(Bitmap &image, std::string imageName, bool clockwise, std::string outputPath) {
    Bitmap rotatedImage = Bitmap(image);

    rotatedImage.rotate(clockwise);
    
    if (!rotatedImage.write(outputPath)) return 0;

    std::cout << imageName << " rotated " << (clockwise ? "clockwise" : "counter-clockwise") << " and saved to " << outputPath << std::endl;

    return 1;
}