# Laboratory Work 1

## Author

Анисимов Василий Сергеевич, группа 24.Б81-мм

## Contacts

st129629@student.spbu.ru

## Description

This project is a C++ program that reads an image from a file, rotates it clockwise and counterclockwise, and applies a Gaussian filter to rotated images. The program then writes the rotated and filtered images to an output directory.

It supports 1-, 8-, 16- and 24-bit both grayscale and colored images. Support of 8-bit colored images is limited.

### Files

- `src/main.cpp`: The main file containing the main function that reads an image from a file, rotates it clockwise and counterclockwise, and applies a Gaussian filter to rotated images.
- `src/bitmap.h`: The header file containing the structure definitions for the BMP file format and the Bitmap class.
- `src/bitmap.cpp`: The implementation file containing the functions for reading, writing, rotating, and filtering images.
- `src/util.h`: Header file containing utility functions for argument parsing, kernel generation, and image manipulation.
- `src/util.cpp`: Implementation file containing the utility functions defined in util.h.
- `Makefile`: The makefile for building the program.

### Build

```
make
```

### Run

```
./main <input image path>
```

### Usage

```
Usage: main [options] [input_image] [output_dir] [num_threads] [kernel_size] [sigma]
Options:
  -i <input_image>     Specify the input image file  (default: image.bmp)
  -o <output_dir>      Specify the output directory  (default: output/)
  -n <num_threads>     Specify the number of threads (default: best possible)
  -k <kernel_size>     Specify the kernel size (default: 5)
  -s <sigma>           Specify the sigma (default: 1.0)
  -h                   Display this help message
If input_image, output_dir, num_threads, kernel_size or sigma are not provided, default values are used.
```