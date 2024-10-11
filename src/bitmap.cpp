#include <cstdio>
#include <iostream>
#include <fstream>
#include "bitmap.h"
#include <cstdlib>


void Bitmap::load(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    unsigned char fileType[2];
    file.read(reinterpret_cast<char*>(fileType), 2);
    
    if ((fileType[0] != 'B') && (fileType[1] != 'M')) {
        std::cout << "Error: File is not a BMP file" << std::endl;
        exit(1);
    }

    bmpFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));

    bmpFileDibInfo dibInfo;
    file.read(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    file.seekg(header.bmpOffset, std::ios::beg);

    for (int x = 0; x < dibInfo.height; x++) {
        std::vector <Pixel> pixelRow;

        for (int y = 0; y < dibInfo.width; y++) {
            int blue = file.get();
            int green = file.get();
            int red = file.get();
            //std::cout << blue << std::endl;
            pixelRow.push_back(Pixel(red, green, blue));

        }

        file.seekg(dibInfo.width % 4, std::ios::cur);

        pixels.insert(pixels.begin(), pixelRow);
        
    }
    file.close();
}


void Bitmap::write(std::string fileName) {
    std::ofstream file(fileName, std::ios::binary);

    file.write("BM", 2);

    bmpFileHeader header = { 0 };

    header.bmpOffset = 2 + sizeof(bmpFileHeader) + sizeof(bmpFileDibInfo);
    header.fileSize = header.bmpOffset + (pixels.size() * 3 + pixels[0].size() % 4) * pixels.size();

    file.write(reinterpret_cast<char*>(&header), sizeof(bmpFileHeader));
    
    bmpFileDibInfo dibInfo = { 0 };
    
    dibInfo.headerSize = sizeof(bmpFileDibInfo);
    dibInfo.width = pixels[0].size();
    dibInfo.height = pixels.size();
    dibInfo.planes = 1;
    dibInfo.bitsPerPixel = 24;
    dibInfo.compression = 0;
    dibInfo.imageSize = dibInfo.width * dibInfo.height * 3;
    dibInfo.xPixelsPerMeter = 2835;
    dibInfo.yPixelsPerMeter = 2835;
    dibInfo.colorsUsed = 0;
    dibInfo.importantColors = 0;
    
    file.write(reinterpret_cast<char*>(&dibInfo), sizeof(bmpFileDibInfo));

    for (int x = pixels.size() - 1; x >= 0; x--) {
        const std::vector <Pixel> & rowPixels = pixels[x];
        
        for (int y = 0; y < pixels[x].size(); y++) {
            const Pixel& pixel = rowPixels[y];
            
            file.put((unsigned char)(pixel.blue));
            file.put((unsigned char)(pixel.green));
            file.put((unsigned char)(pixel.red));
        }

        for (int i = 0; i < rowPixels.size() % 4; i++) {
            file.put(0);
        }
    }

    file.close();
}