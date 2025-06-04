#include <gtest/gtest.h>
#include "bitmap.h"
#include "util.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

bool pixelEqual(const Pixel& a, const Pixel& b, int tol=0) {
    return std::abs(a.red - b.red) <= tol && std::abs(a.green - b.green) <= tol && std::abs(a.blue - b.blue) <= tol;
}

bool imageEqual(const Bitmap& a, const Bitmap& b, int tol=0) {
    const auto& pa = a.getPixels();
    const auto& pb = b.getPixels();
    if (pa.size() != pb.size() || pa[0].size() != pb[0].size()) return true;
    for (size_t i = 0; i < pa.size(); ++i)
        for (size_t j = 0; j < pa[0].size(); ++j)
            if (!pixelEqual(pa[i][j], pb[i][j], tol))
                return false;
    return true;
}

TEST(BitmapTest, LoadValidBmp) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
}

TEST(BitmapTest, LoadInvalidFile) {
    Bitmap bmp;
    ASSERT_FALSE(bmp.load("not_a_real_file.bmp"));
}

TEST(BitmapTest, LoadUnsupportedFormat) {
    Bitmap bmp;
    std::ofstream("output/not_bmp.txt") << "hello";
    ASSERT_FALSE(bmp.load("output/not_bmp.txt"));
}

TEST(BitmapTest, WriteAndReload) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    ASSERT_TRUE(bmp.write("output/test_write.bmp"));
    Bitmap bmp2;
    ASSERT_TRUE(bmp2.load("output/test_write.bmp"));
    
    ASSERT_TRUE(imageEqual(bmp, bmp2));
}

TEST(BitmapTest, RotateClockwiseAndCounter) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    Bitmap orig = bmp;
    bmp.rotate(true); // clockwise
    bmp.rotate(false); // counterclockwise
    // Rotating CW then CCW should return to original
    ASSERT_TRUE(imageEqual(bmp, orig));
}

TEST(BitmapTest, RotateFourTimes) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    Bitmap orig = bmp;
    for (int i = 0; i < 4; ++i) bmp.rotate(true);
    ASSERT_TRUE(imageEqual(bmp, orig));
}

TEST(BitmapTest, CopyConstructor) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    Bitmap copy = bmp;
    ASSERT_TRUE(imageEqual(bmp, copy));
}

TEST(BitmapTest, GaussianFilterNoCrash) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    auto kernel = generateGaussianKernel(5, 1.0);
    bmp.applyGaussianFilter(kernel, 2);
    ASSERT_TRUE(bmp.write("output/test_filtered.bmp"));
}

TEST(BitmapTest, GaussianFilterEffect) {
    Bitmap bmp;
    ASSERT_TRUE(bmp.load("24.bmp"));
    Bitmap orig = bmp;
    auto kernel = generateGaussianKernel(5, 1.0);
    bmp.applyGaussianFilter(kernel, 2);
    
    ASSERT_FALSE(imageEqual(bmp, orig));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 