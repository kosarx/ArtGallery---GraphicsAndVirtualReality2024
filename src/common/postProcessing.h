#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <fstream>
#include <vector>
//#include <Fade_2D.h>
#include <glm/glm.hpp>

//using namespace GEOM_FADE2D;

void Floyd_SteinbergDitheringAlgorithm_RGBA(GLubyte* pixels, int width, int height, bool grayscale);
void Floyd_SteinbergDitheringAlgorithm_RGB(GLubyte* pixels, int width, int height);
GLubyte* applyTextureFloyd_SteinbergDithering(GLuint texture,  int width, int height);

struct Point2 {
    float x;
    float y;

    // Constructor to initialize Point2 with x and y values
    Point2(float xVal, float yVal) : x(xVal), y(yVal) {}
};
struct Dot {
    Point2 point;
    int radius;
};
std::vector<Dot> sampleDarkPoints(const GLubyte* img, const int width, const int height, const int numPoints, std::vector<std::pair<int, int>>& darkPixels);
std::vector<Dot> samplePoints(const GLubyte* img, const int width, const int height, const int numPoints, std::vector<std::pair<int, int>>& chosenPixels);
void drawDots(GLubyte* img, const int width, const int height, const std::vector<Dot>& dots);
std::vector<Dot> processImageForPointilism(const GLubyte* imageData, int width, int height, int numSamples, bool useRealTimeRendering);

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType{ 0x4D42 };  // "BM"
    uint32_t fileSize{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t offsetData{ 0 };
};

struct BMPInfoHeader {
    uint32_t size{ 0 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bitCount{ 0 };
    uint32_t compression{ 0 };
    uint32_t sizeImage{ 0 };
    int32_t xPixelsPerMeter{ 0 };
    int32_t yPixelsPerMeter{ 0 };
    uint32_t colorsUsed{ 0 };
    uint32_t colorsImportant{ 0 };
};
#pragma pack(pop)
void saveTextureAsBMP(GLuint textureID, int width, int height, const char* filename);

#endif // POSTPROCESSING_H