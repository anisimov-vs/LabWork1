# Laboratory Work 1

## Author
Vasily Anisimov, Group 24.B81-mm

## Contacts

st129629@student.spbu.ru

## Description

This project is a C++ program that reads an image from a file, rotates it clockwise and counterclockwise, and applies a Gaussian filter to it. The program then writes the rotated and filtered images to an output directory.

### Files

- `src/main.cpp`: The main file containing the main function and other functions for reading, writing, rotating, and filtering images.
- `src/bitmap.h`: The header file containing the structure definitions for the BMP file format and the Bitmap class.
- `src/bitmap.cpp`: The implementation file containing the functions for reading, writing, rotating, and filtering images.
- `Makefile`: The makefile for building the program.

### Build

```
make
```

### Run

```
./main -i image.bmp -o output/
```

### Usage

```
Usage: main [options] [input_image] [output_dir]
Options:
  -i <input_image>    Specify the input image file (default: image.bmp)
  -o <output_dir>     Specify the output directory (default: output/)
  -h                  Display this help message
If input_image and output_dir are not provided, default values are used.
Default input_image: image.bmp
Default output_dir: output/
```

```
./main -i input.bmp -o output/
```