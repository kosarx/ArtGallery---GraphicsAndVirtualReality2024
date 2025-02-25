#include <iostream>
#include <vector>
#include <cstdlib> // For rand() and srand()
//#include <ctime>   // For time()
#include <random> // for shuffle
//#include <algorithm>
#include <chrono>
#include <bitset>
#include <unordered_map>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "postProcessing.h"

//    for (int y = 0; y < height; y++) {
    //        for (int x = 0; x < width; x++) {
    //            int index = (y * elems_per_row + x * 4);
    //            glm::vec4 oldColor(
    //                pixels[index],
    //                pixels[index + 1],
    //                pixels[index + 2],
    //                pixels[index + 3]
    //            );
    //
    //            glm::vec4 quantizedColor(
    //                std::round(oldColor.r / 255.0f * quantizationFactor) / quantizationFactor * 255.0f,
    //                std::round(oldColor.g / 255.0f * quantizationFactor) / quantizationFactor * 255.0f,
    //                std::round(oldColor.b / 255.0f * quantizationFactor) / quantizationFactor * 255.0f,
    //                oldColor.a
    //            );
    //
    //            glm::vec4 error = oldColor - quantizedColor;
    //
    //            pixels[index] = static_cast<GLubyte>(quantizedColor.r);
    //            pixels[index + 1] = static_cast<GLubyte>(quantizedColor.g);
    //            pixels[index + 2] = static_cast<GLubyte>(quantizedColor.b);
    //
    //
    //            if (x + 1 < width) {
    //                int rightIndex = index + 4;
    //                pixels[rightIndex] = std::min(255, std::max(0, pixels[rightIndex] + static_cast<int>(error.r * 7 / 16)));
    //                pixels[rightIndex + 1] = std::min(255, std::max(0, pixels[rightIndex + 1] + static_cast<int>(error.g * 7 / 16)));
    //                pixels[rightIndex + 2] = std::min(255, std::max(0, pixels[rightIndex + 2] + static_cast<int>(error.b * 7 / 16)));
    //            }
    //            if (y + 1 < height) {
    //                int bottomIndex = index + elems_per_row;
    //                if (x > 0) {
    //                    int bottomLeftIndex = bottomIndex - 4;
    //                    pixels[bottomLeftIndex] = std::min(255, std::max(0, pixels[bottomLeftIndex] + static_cast<int>(error.r * 3 / 16)));
    //                    pixels[bottomLeftIndex + 1] = std::min(255, std::max(0, pixels[bottomLeftIndex + 1] + static_cast<int>(error.g * 3 / 16)));
    //                    pixels[bottomLeftIndex + 2] = std::min(255, std::max(0, pixels[bottomLeftIndex + 2] + static_cast<int>(error.b * 3 / 16)));
    //                }
    //                pixels[bottomIndex] = std::min(255, std::max(0, pixels[bottomIndex] + static_cast<int>(error.r * 5 / 16)));
    //                pixels[bottomIndex + 1] = std::min(255, std::max(0, pixels[bottomIndex + 1] + static_cast<int>(error.g * 5 / 16)));
    //                pixels[bottomIndex + 2] = std::min(255, std::max(0, pixels[bottomIndex + 2] + static_cast<int>(error.b * 5 / 16)));
    //                if (x + 1 < width) {
    //                    int bottomRightIndex = bottomIndex + 4;
    //                    pixels[bottomRightIndex] = std::min(255, std::max(0, pixels[bottomRightIndex] + static_cast<int>(error.r * 1 / 16)));
    //                    pixels[bottomRightIndex + 1] = std::min(255, std::max(0, pixels[bottomRightIndex + 1] + static_cast<int>(error.g * 1 / 16)));
    //                    pixels[bottomRightIndex + 2] = std::min(255, std::max(0, pixels[bottomRightIndex + 2] + static_cast<int>(error.b * 1 / 16)));
    //                }
    //            }
    //        }
    //    }
    //}
    //====== VERSION 2 =================
    //    // Precompute quantization multiplier
    //    float quantMult = 255.0f / quantizationFactor;
    //
    //    // Block size for cache blocking
    //    const int blockSize = 1024*64; // Can be tuned based on the cache size of the target hardware
    //
    //    for (int y = 0; y < height; y += blockSize) {
    //        for (int x = 0; x < width; x += blockSize) {
    //            int blockHeight = std::min(blockSize, height - y);
    //            int blockWidth = std::min(blockSize, width - x);
    //
    //            for (int by = 0; by < blockHeight; ++by) {
    //                for (int bx = 0; bx < blockWidth; ++bx) {
    //                    int pixelY = y + by;
    //                    int pixelX = x + bx;
    //                    int index = (pixelY * elems_per_row + pixelX * 4);
    //
    //                    // Load current pixel color
    //                    float oldColorR = pixels[index];
    //                    float oldColorG = pixels[index + 1];
    //                    float oldColorB = pixels[index + 2];
    //                    float oldColorA = pixels[index + 3];
    //
    //                    // Quantize colors
    //                    float quantizedColorR = std::round(oldColorR / 255.0f * quantizationFactor) / quantizationFactor * 255.0f;
    //                    float quantizedColorG = std::round(oldColorG / 255.0f * quantizationFactor) / quantizationFactor * 255.0f;
    //                    float quantizedColorB = std::round(oldColorB / 255.0f * quantizationFactor) / quantizationFactor * 255.0f;
    //
    //                    // Compute error
    //                    float errorR = oldColorR - quantizedColorR;
    //                    float errorG = oldColorG - quantizedColorG;
    //                    float errorB = oldColorB - quantizedColorB;
    //
    //                    // Store quantized color back
    //                    pixels[index] = static_cast<GLubyte>(quantizedColorR);
    //                    pixels[index + 1] = static_cast<GLubyte>(quantizedColorG);
    //                    pixels[index + 2] = static_cast<GLubyte>(quantizedColorB);
    //
    //                    // Spread error to neighboring pixels using Floyd-Steinberg weights
    //                    if (pixelX + 1 < width) {
    //                        int rightIndex = index + 4;
    //                        pixels[rightIndex] = CLAMP(pixels[rightIndex] + static_cast<int>(errorR * 7 / 16));
    //                        pixels[rightIndex + 1] = CLAMP(pixels[rightIndex + 1] + static_cast<int>(errorG * 7 / 16));
    //                        pixels[rightIndex + 2] = CLAMP(pixels[rightIndex + 2] + static_cast<int>(errorB * 7 / 16));
    //                    }
    //                    if (pixelY + 1 < height) {
    //                        int bottomIndex = index + elems_per_row;
    //                        if (pixelX > 0) {
    //                            int bottomLeftIndex = bottomIndex - 4;
    //                            pixels[bottomLeftIndex] = CLAMP(pixels[bottomLeftIndex] + static_cast<int>(errorR * 3 / 16));
    //                            pixels[bottomLeftIndex + 1] = CLAMP(pixels[bottomLeftIndex + 1] + static_cast<int>(errorG * 3 / 16));
    //                            pixels[bottomLeftIndex + 2] = CLAMP(pixels[bottomLeftIndex + 2] + static_cast<int>(errorB * 3 / 16));
    //                        }
    //                        pixels[bottomIndex] = CLAMP(pixels[bottomIndex] + static_cast<int>(errorR * 5 / 16));
    //                        pixels[bottomIndex + 1] = CLAMP(pixels[bottomIndex + 1] + static_cast<int>(errorG * 5 / 16));
    //                        pixels[bottomIndex + 2] = CLAMP(pixels[bottomIndex + 2] + static_cast<int>(errorB * 5 / 16));
    //                        if (pixelX + 1 < width) {
    //                            int bottomRightIndex = bottomIndex + 4;
    //                            pixels[bottomRightIndex] = CLAMP(pixels[bottomRightIndex] + static_cast<int>(errorR * 1 / 16));
    //                            pixels[bottomRightIndex + 1] = CLAMP(pixels[bottomRightIndex + 1] + static_cast<int>(errorG * 1 / 16));
    //                            pixels[bottomRightIndex + 2] = CLAMP(pixels[bottomRightIndex + 2] + static_cast<int>(errorB * 1 / 16));
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

// Helper macros for clamping values between 0 and 255
#define CLAMP(val) (std::min(255, std::max(0, ((int) val))))
void _Floyd_SteinbergDitheringGrayscale_RGBA(GLubyte* pixels, int width, int height, float quantizationFactor, size_t elems_per_row) {
    // Temporary staging arrays for error diffusion
    std::vector<float> error(width + 2, 0.0f);

    // Precompute quantization multiplier
    float quantMult = 255.0f / quantizationFactor;
    float invQuantMult = 1.0f / quantMult;

    // Block size for cache blocking
    const int blockSize = 1024 * 256; // Can be tuned based on the cache size of the target hardware

    // Reusable vector for next error
    std::vector<float> nextError(width + 2, 0.0f);

    for (int y = 0; y < height; y += blockSize) {
        for (int x = 0; x < width; x += blockSize) {
            int blockHeight = std::min(blockSize, height - y);
            int blockWidth = std::min(blockSize, width - x);

            for (int by = 0; by < blockHeight; ++by) {
                // Clear the next error vector
                std::fill(nextError.begin(), nextError.end(), 0.0f);

                for (int bx = 0; bx < blockWidth; ++bx) {
                    int pixelY = y + by;
                    int pixelX = x + bx;
                    int index = (pixelY * elems_per_row + pixelX * 4);

                    // Load current pixel color and convert to grayscale
                    float oldColorR = pixels[index];
                    float oldColorG = pixels[index + 1];
                    float oldColorB = pixels[index + 2];
                    float oldGray = 0.299f * oldColorR + 0.587f * oldColorG + 0.114f * oldColorB + error[pixelX + 1];

                    // Quantize grayscale color
                    float quantizedGray = std::round(oldGray * invQuantMult) * quantMult;

                    // Compute error
                    float errorVal = oldGray - quantizedGray;

                    // Store quantized grayscale color back
                    GLubyte grayValue = static_cast<GLubyte>(quantizedGray);
                    pixels[index] = grayValue;
                    pixels[index + 1] = grayValue;
                    pixels[index + 2] = grayValue;

                    // Spread error to neighboring pixels using Floyd-Steinberg weights
                    error[pixelX + 2] += errorVal * 7 / 16;
                    nextError[pixelX] += errorVal * 3 / 16;
                    nextError[pixelX + 1] += errorVal * 5 / 16;
                    nextError[pixelX + 2] += errorVal * 1 / 16;
                }

                // Move to the next row
                error.swap(nextError);
            }
        }
    }
}

void _Floyd_SteinbergDitheringColored_RGBA(GLubyte* pixels, int width, int height, float quantizationFactor, size_t elems_per_row) {
    // ====== VERSION 3 =================
    // Temporary staging arrays for error diffusion
    std::vector<float> errorR(width + 2, 0.0f);
    std::vector<float> errorG(width + 2, 0.0f);
    std::vector<float> errorB(width + 2, 0.0f);

    // Precompute quantization multiplier
    float quantMult = 255.0f / quantizationFactor;
    float invQuantMult = 1.0f / quantMult;

    // Block size for cache blocking
    const int blockSize = 1024 * 256; // Can be tuned based on the cache size of the target hardware

    for (int y = 0; y < height; y += blockSize) {
        for (int x = 0; x < width; x += blockSize) {
            int blockHeight = std::min(blockSize, height - y);
            int blockWidth = std::min(blockSize, width - x);

            for (int by = 0; by < blockHeight; ++by) {
                std::vector<float> nextErrorR(width + 2, 0.0f);
                std::vector<float> nextErrorG(width + 2, 0.0f);
                std::vector<float> nextErrorB(width + 2, 0.0f);

                for (int bx = 0; bx < blockWidth; ++bx) {
                    int pixelY = y + by;
                    int pixelX = x + bx;
                    int index = (pixelY * elems_per_row + pixelX * 4);

                    // Load current pixel color
                    float oldColorR = pixels[index] + errorR[pixelX + 1];
                    float oldColorG = pixels[index + 1] + errorG[pixelX + 1];
                    float oldColorB = pixels[index + 2] + errorB[pixelX + 1];

                    // Quantize colors
                    float quantizedColorR = std::round(oldColorR * invQuantMult) * quantMult;
                    float quantizedColorG = std::round(oldColorG * invQuantMult) * quantMult;
                    float quantizedColorB = std::round(oldColorB * invQuantMult) * quantMult;

                    // Compute error
                    float errorRVal = oldColorR - quantizedColorR;
                    float errorGVal = oldColorG - quantizedColorG;
                    float errorBVal = oldColorB - quantizedColorB;

                    // Store quantized color back

                    pixels[index] = static_cast<GLubyte>(quantizedColorR);
                    pixels[index + 1] = static_cast<GLubyte>(quantizedColorG);
                    pixels[index + 2] = static_cast<GLubyte>(quantizedColorB);

                    // Spread error to neighboring pixels using Floyd-Steinberg weights
                    errorR[pixelX + 2] += errorRVal * 7 / 16;
                    errorG[pixelX + 2] += errorGVal * 7 / 16;
                    errorB[pixelX + 2] += errorBVal * 7 / 16;

                    nextErrorR[pixelX] += errorRVal * 3 / 16;
                    nextErrorG[pixelX] += errorGVal * 3 / 16;
                    nextErrorB[pixelX] += errorBVal * 3 / 16;

                    nextErrorR[pixelX + 1] += errorRVal * 5 / 16;
                    nextErrorG[pixelX + 1] += errorGVal * 5 / 16;
                    nextErrorB[pixelX + 1] += errorBVal * 5 / 16;

                    nextErrorR[pixelX + 2] += errorRVal * 1 / 16;
                    nextErrorG[pixelX + 2] += errorGVal * 1 / 16;
                    nextErrorB[pixelX + 2] += errorBVal * 1 / 16;
                }

                // Move to the next row
                errorR.swap(nextErrorR);
                errorG.swap(nextErrorG);
                errorB.swap(nextErrorB);
            }
        }
    }
}

#undef CLAMP

void Floyd_SteinbergDitheringAlgorithm_RGBA(GLubyte* pixels, int width, int height, bool grayscale = false) {
    std::cout << "GLubyte Floyd-Steinberg Dithering...";
    // start the timer
    auto start = std::chrono::high_resolution_clock::now();
    size_t elems_per_row = width * 4; // 4 components per pixel (RGBA)
    int bits =2; 
    float quantizationFactor = (1 << bits) - 1;

    if (grayscale) {
		_Floyd_SteinbergDitheringGrayscale_RGBA(pixels, width, height, quantizationFactor, elems_per_row);
	}
	else {
		_Floyd_SteinbergDitheringColored_RGBA(pixels, width, height, quantizationFactor, elems_per_row);
	}

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "...Finished. Took: " << elapsed_seconds.count() << "s." << std::endl;
    /*std::cout << "...Finished." << std::endl;*/

}


void Floyd_SteinbergDitheringAlgorithm_RGB(GLubyte* pixels, int text_width, int text_height) {
    std::cout << "GLubyte Floyd-Steinberg Dithering...";
    unsigned int counter = 0;

    counter = 0;
    size_t elems_per_row = text_width * 3;
    size_t elems_per_col = text_height;
    int bits = 2;
    float quantizationFactor = (1 << bits) - 1;

    for (size_t col = 0; col < elems_per_col; col++) {
        for (size_t row = 0; row < elems_per_row; row = row + 3) { // row = row + 3, because of RGB format, so row will always point to 
            //the first r element of pixel
            int index = (col * elems_per_row + row);
            glm::ivec3 oldColor(
                pixels[index],       //R old
                pixels[index + 1],   //G old
                pixels[index + 2]    //B old
            );

            //GLubyte grayscale = (GLubyte)(0.2126 * pixels[index] + 0.7152 * pixels[index + 1] + 0.0722 * pixels[index + 2]);
            // Quantize each color channel individually
            GLubyte quantizedR = static_cast<GLubyte>(std::round(float(oldColor.r) / 255.0f * quantizationFactor) / quantizationFactor * 255.0f);
            GLubyte quantizedG = static_cast<GLubyte>(std::round(float(oldColor.g) / 255.0f * quantizationFactor) / quantizationFactor * 255.0f);
            GLubyte quantizedB = static_cast<GLubyte>(std::round(float(oldColor.b) / 255.0f * quantizationFactor) / quantizationFactor * 255.0f);

            glm::ivec3 quantizedColor(
                quantizedR, //R new
                quantizedG, //G new
                quantizedB  //B new
            );

            //calculate the error
            glm::ivec3 error = oldColor - quantizedColor;

            //applying the quantized color to the texture
            
            pixels[index] = quantizedR;
            pixels[index + 1] = quantizedR; //quantizedG
            pixels[index + 2] = quantizedR; //quantizedB
            
            // Propagate the error to neighboring pixels
            if (row + 3 < elems_per_row) {
                // Right pixel
                pixels[index + 3] += error.r * 7 / 16;
                pixels[index + 4] += error.g * 7 / 16;
                pixels[index + 5] += error.b * 7 / 16;
            }
            if (col + 1 < elems_per_col) {
                if (row > 0) {
                    // Bottom-left pixel
                    pixels[index + elems_per_row - 3] += error.r * 3 / 16;
                    pixels[index + elems_per_row - 2] += error.g * 3 / 16;
                    pixels[index + elems_per_row - 1] += error.b * 3 / 16;
                }
                // Bottom pixel
                pixels[index + elems_per_row] += error.r * 5 / 16;
                pixels[index + elems_per_row + 1] += error.g * 5 / 16;
                pixels[index + elems_per_row + 2] += error.b * 5 / 16;
                if (row + 3 < elems_per_row) {
                    // Bottom-right pixel
                    pixels[index + elems_per_row + 3] += error.r * 1 / 16;
                    pixels[index + elems_per_row + 4] += error.g * 1 / 16;
                    pixels[index + elems_per_row + 5] += error.b * 1 / 16;
                }
            }
            counter++;
        }
    }
    std::cout << "... Counted: " << counter << "... ... Finished." << std::endl;
}

GLubyte* applyTextureFloyd_SteinbergDithering(GLuint texture, int text_width, int text_height) {
    std::cout << "GLuint Floyd-Steinberg Dithering...";
    //getting texture image
    GLubyte* imageData = new GLubyte[text_width * text_height * 3]; // 3 for RGB, 4 for RGBA
    std::cout << "Created imageData" << std::endl;
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture);
    std::cout << "Getting TexImage...";
    // create a timer for GetTexImage
    auto start = std::chrono::high_resolution_clock::now();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData); //last parameter 0 when reading into a PBO
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    //std::cout << "Got, going to apply FSD" << std::endl;
    std::cout << "Got, took: " << elapsed_seconds.count() << "s. Going to apply FSD...." << std::endl;
    // getting the Floyd-Steinberg Dithering texture of the morningRoomTexture
    auto start2 = std::chrono::high_resolution_clock::now();
    Floyd_SteinbergDitheringAlgorithm_RGB(imageData, text_width, text_height);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2 - start2;
    std::cout << "applied in" << elapsed_seconds2.count() << "s." << std::endl;
    //std::cout << "Creating dither thread... " << endl;									// 8192 is the size of tex_u1_v1.jpg for morningRoomTexture
    //std::thread DitheringTextureThread(applyFloyd_SteinbergDithering, imageData, 8192, 8192); //we pass the texture as a reference
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, text_width, text_height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    return imageData;
}

//void samplePoints(GLubyte* img, const int width, const int height, const int numPoints) {
//    // Set the random seed
//    srand(static_cast<unsigned int>(time(0)));
//
//    // Deep copy the image data
//    GLubyte* imgCopy = new GLubyte[width * height * 4];
//    memcpy(imgCopy, img, width * height * 4);
//    // Turn the copied image completely white
//    memset(imgCopy, 255, width * height * 4);
//
//    std::vector<std::pair<int, int>> darkPixels;
//
//    // Collect all dark pixels
//    for (int j = 0; j < height; j++) {
//        for (int i = 0; i < width; i++) {
//            int pixelIndex = 4 * (j * width + i);
//            int brightness = 0.2126f * img[pixelIndex] + 0.7152f * img[pixelIndex + 1] + 0.0722f * img[pixelIndex + 2];
//
//            if (brightness < 128) {
//                darkPixels.emplace_back(i, j);
//            }
//        }
//    }
//
//    // Randomly select up to numPoints dark pixels
//    int pointsToDraw = std::min(numPoints, static_cast<int>(darkPixels.size()));
//    std::random_shuffle(darkPixels.begin(), darkPixels.end());
//
//    for (int k = 0; k < pointsToDraw; k++) {
//        int i = darkPixels[k].first;
//        int j = darkPixels[k].second;
//
//        int pixelIndex = 4 * (j * width + i);
//        int brightness = 0.2126f * img[pixelIndex] + 0.7152f * img[pixelIndex + 1] + 0.0722f * img[pixelIndex + 2];
//
//        // Calculate radius based on brightness (darker pixel, larger radius)
//        int radius = static_cast<int>(2 + (128 - brightness) / 128.0f * 10); // Ranges from 2 to 12
//        int radiusSquared = radius * radius;
//
//        for (int dy = -radius; dy <= radius; dy++) {
//            int y = j + dy;
//            if (y < 0 || y >= height) continue;
//            for (int dx = -radius; dx <= radius; dx++) {
//                int x = i + dx;
//                if (x < 0 || x >= width) continue;
//                if (dx * dx + dy * dy <= radiusSquared) {
//                    int circlePixelIndex = 4 * (y * width + x);
//                    imgCopy[circlePixelIndex] = 0;
//                    imgCopy[circlePixelIndex + 1] = 0;
//                    imgCopy[circlePixelIndex + 2] = 0;
//                }
//            }
//        }
//    }
//
//    // Copy the modified image data back to the original image
//    memcpy(img, imgCopy, width * height * 4);
//
//    // Clean up
//    delete[] imgCopy;
//
//    std::cout << "Pointillism effect applied!" << std::endl;
//}

//void drawDots(GLubyte* img, const int width, const int height){// , const std::vector<Dot>& dots) {
//	//
//}

std::vector<Dot> sampleDarkPoints(const GLubyte* img, const int width, const int height, const int numPoints, std::vector<std::pair<int, int>>& darkPixels) {
    // Set the random seed
    //srand(static_cast<unsigned int>(time(0)));
    srand(42);

    /*std::vector<std::pair<int, int>> darkPixels;*/
    std::vector<Dot> dots;

    // Reserve space for vectors to avoid multiple reallocations
    darkPixels.reserve(width * height);
    dots.reserve(numPoints);

    // Collect all dark pixels
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int pixelIndex = 4 * (j * width + i);
            int brightness = 0.2126f * img[pixelIndex] + 0.7152f * img[pixelIndex + 1] + 0.0722f * img[pixelIndex + 2];

            if (brightness < 150) { // < 128
                darkPixels.emplace_back(i, j);
            }
        }
    }

    // Randomly select up to numPoints dark pixels
    int pointsToDraw = std::min(numPoints, static_cast<int>(darkPixels.size()));
    // Shuffle the darkPixels vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(darkPixels.begin(), darkPixels.end(), g);

    // Collect the dark pixels
    for (int k = 0; k < pointsToDraw; k++) {
        int i = darkPixels[k].first;
        int j = darkPixels[k].second;

        int pixelIndex = 4 * (j * width + i);
        int brightness = 0.2126f * img[pixelIndex] + 0.7152f * img[pixelIndex + 1] + 0.0722f * img[pixelIndex + 2]; // Range from 0 to 255

        // Calculate radius based on brightness (darker pixel, larger radius)
        int radius = static_cast<int>(2 + (128 - brightness) / 128.0f * 10); // Range from 2 to 12
        // debugging: brighter pixel, larger radius
        //int radius = static_cast<int>(2 + (brightness - 128) / 128.0f * 10); // Range from 2 to 12

        //Create the Point2 struct
		Point2 point(i, j);

        // Store the dot information
        dots.push_back(Dot{ point, radius });
    }

    return dots;
}

void drawDots(GLubyte* img, const int width, const int height, const std::vector<Dot>& dots) {
    // Define a bitset to keep track of drawn pixels
    std::vector<std::bitset<1>> drawnPixels(width * height, 0);
    
    //copy the image data
    //GLubyte* imgCopy = new GLubyte[width * height * 4];
    //memcpy(imgCopy, img, width * height * 4);
    
    // Turn the image completely white before drawing the dots
    //memset(img, 255, width * height * 4); // needs black dots
    //Turn the image completely black before drawing the dots
    //memset(img, 0, width * height * 4); // needs white dots

    // Draw the dots
    for (const Dot& dot : dots) {
        int radiusSquared = dot.radius * dot.radius;
        int startY = std::max(0, static_cast<int>(dot.point.y) - dot.radius);
        int endY = std::min(height - 1, static_cast<int>(dot.point.y) + dot.radius);
        int startX = std::max(0, static_cast<int>(dot.point.x) - dot.radius);
        int endX = std::min(width - 1, static_cast<int>(dot.point.x) + dot.radius);

        for (int y = startY; y <= endY; y++) {
            int dy = y - dot.point.y;
            for (int x = startX; x <= endX; x++) {
                int dx = x - dot.point.x;
                if (dx * dx + dy * dy <= radiusSquared) {
                    int pixelIndex = y * width + x;
                    if (!drawnPixels[pixelIndex].test(0)) {
                        int imgIndex = 4 * pixelIndex;
                        img[imgIndex] = 0;
                        img[imgIndex + 1] = 0;
                        img[imgIndex + 2] = 0;
                        drawnPixels[pixelIndex].set(0);
                        //GLubyte color = imgCopy[pixelIndex];
                        //img[pixelIndex] = imgCopy[pixelIndex];
                        //img[pixelIndex + 1] = imgCopy[pixelIndex + 1];
                        //img[pixelIndex + 2] = imgCopy[pixelIndex + 2];
                        // Optionally, set the alpha value to fully opaque if needed
                        // img[pixelIndex + 3] = 255;
                    }
                }
            }
        }
    }
}

std::vector<Dot> processImageForPointilism(const GLubyte* imageData, int width, int height, int numSamples, bool useRealTimeRendering) {
    // Define a vector to store the dark pixels
    //std::vector<std::pair<int, int>> darkPixels;
    std::vector<std::pair<int, int>> chosenPixels;

    // Sample points from the image
    std::vector<Dot> dots = sampleDarkPoints(imageData, width, height, numSamples, chosenPixels); // image data, width, height, number of points to sample, vector to store dark pixels

    if (useRealTimeRendering && false) { // Disabled, would produce weighted Voronoi stippling
        try {
            // Calculate the Voronoi diagram of the sampled points
            //Fade_2D dt;
            //std::vector<Point2> points;
            //points.reserve(dots.size()); // Reserve space to avoid reallocations
            //for (const Dot& dot : dots) {
            //    points.emplace_back(dot.point.x(), dot.point.y());
            //}
            //dt.insert(points);

            //// Create Voronoi diagram
            //Voronoi2* voronoi = dt.getVoronoiDiagram();
            //if (!voronoi) {
            //    std::cerr << "Error creating Voronoi diagram." << std::endl;
            //    return dots;
            //}

            //std::vector<VoroCell2*> cells;
            //voronoi->getVoronoiCells(cells);
            //std::cout << "Voronoi cells retrieved: " << cells.size() << std::endl;

            //for (size_t i = 0; i < cells.size(); ++i) {
            //    cells[i]->setCustomCellIndex(static_cast<int>(i));
            //}

            //// Create a map to store dark pixels by cell index
            //std::unordered_map<int, std::vector<std::pair<int, int>>> darkPixelsByCell;
            //darkPixelsByCell.reserve(cells.size()); // Reserve space to avoid reallocations
            //for (const auto& darkPixel : chosenPixels) {
            //    VoroCell2* cell = voronoi->locateVoronoiCell(Point2(darkPixel.first, darkPixel.second));
            //    if (cell) {
            //        int cellIndex = cell->getCustomCellIndex();
            //        darkPixelsByCell[cellIndex].emplace_back(darkPixel);
            //    }
            //}

            //// Create the weighted Voronoi diagram
            //for (VoroCell2* cell : cells) {
            //    // Get the dark pixels in the cell
            //    auto it = darkPixelsByCell.find(cell->getCustomCellIndex());
            //    if (it == darkPixelsByCell.end()) continue;

            //    const auto& darkPixelsInCell = it->second;

            //    // Calculate the centroid of the cell
            //    Point2 centroid;
            //    double area = cell->getCentroid(centroid);
            //    if (area > 0 && !darkPixelsInCell.empty()) {
            //        // Calculate the weighted centroid
            //        Point2 weightedCentroid(0, 0);
            //        double totalWeight = 0;
            //        for (const auto& darkPixel : darkPixelsInCell) {
            //            float dx = darkPixel.first - centroid.x();
            //            float dy = darkPixel.second - centroid.y();
            //            float distance = sqrt(dx * dx + dy * dy);
            //            double weight = 1.0 / (1.0 + distance);
            //            weightedCentroid = Point2(weightedCentroid.x() + darkPixel.first * weight, weightedCentroid.y() + darkPixel.second * weight);
            //            totalWeight += weight;
            //        }
            //        weightedCentroid = Point2(weightedCentroid.x() / totalWeight, weightedCentroid.y() / totalWeight);

            //        // Directly adjust the centroid to the new weighted position
            //        centroid = weightedCentroid;
            //    }

            //    // Update the dot position
            //    for (Dot& dot : dots) {
            //        if (Point2(dot.point.x(), dot.point.y()) == centroid) {
            //            dot.point = Point2(centroid.x(), centroid.y());
            //            break;
            //        }
            //    }
            //}

            // Cleanup
            //delete voronoi;
        }
        catch (const std::exception& e) {
            std::cerr << "Error calculating Voronoi diagram: " << e.what() << std::endl;
        }
    }

    return dots;
}

void saveTextureAsBMP(GLuint textureID, int width, int height, const char* filename) {
    std::cout << "Saving texture as BMP..." << std::endl;
    // start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate memory to store the texture data
    std::vector<GLubyte> pixels(width * height * 4); // Assuming RGBA format

    // Read the texture data
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Open a file stream for binary writing
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // BMP File Header
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    infoHeader.size = sizeof(BMPInfoHeader);
    infoHeader.width = width;
    infoHeader.height = -height; // Top-down image
    infoHeader.bitCount = 32; // 4 channels (RGBA)
    infoHeader.compression = 0; // No compression
    infoHeader.sizeImage = width * height * 4;

    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + infoHeader.sizeImage;
    fileHeader.offsetData = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    // Write headers
    outFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    outFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

    // Write pixel data
    outFile.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

    outFile.close();

    if (outFile.good()) {
        std::cout << "Texture saved successfully to " << filename << std::endl;
    }
    else {
        std::cerr << "Error occurred while writing to " << filename << std::endl;
    }

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // end the timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Saved in " << elapsed_seconds.count() << "s." << std::endl;
}