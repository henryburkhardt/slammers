#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "src/fast.hpp"

using namespace std;

void learnedFast(std::vector<cv::KeyPoint>& keypoints, const cv::Mat& img, int threshold, bool nonMaxSuppression = true) {
    unsigned char* raw_data = img.data;
    int height = img.size().height;
    int width = img.size().width;
    int prod = height * width;
    uint *nmsVals = (uint *) malloc(sizeof(uint) * prod);
    for (int i = 0; i < prod; i++) nmsVals[i] = 0;
    #define GENERATE_NMS_VALS() \
        unsigned int absoluteDiffSum = 0; \
        size_t center = row * width + col; \
        size_t row0 = center - 3 * width; \
        size_t row1 = row0 + width; \
        size_t row2 = row1 + width; \
        size_t row3 = row2 + width; \
        size_t row4 = row3 + width; \
        size_t row5 = row4 + width; \
        size_t row6 = row5 + width; \
        absoluteDiffSum += abs(raw_data[row0 - 1] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row0] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row0 + 1] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row1 - 2] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row1 + 2] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row2 - 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row2 + 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row3 - 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row3 + 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row4 - 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row4 + 3] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row5 - 2] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row5 + 2] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row6 - 1] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row6] - comp_pixel); \
        absoluteDiffSum += abs(raw_data[row6 + 1] - comp_pixel); \
        nmsVals[center] = absoluteDiffSum;
    
    #pragma omp parallel for
    for (int row = 3; row < height - 3; row++) {
        for (int col = 3; col < width - 3; col++) {
            int comp_pixel, pixel;
            comp_pixel = raw_data[row * width + col];
            pixel = raw_data[(row + 1) * width + col + 3];
            if (pixel + threshold < comp_pixel) {
                pixel = raw_data[(row - 3) * width + col];
                if (pixel + threshold < comp_pixel) {
                    pixel = raw_data[(row - 1) * width + col - 3];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row - 1) * width + col + 3];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                GENERATE_NMS_VALS();
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                                else {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                                        if (pixel + threshold < comp_pixel) {
                                                                            GENERATE_NMS_VALS();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                                else if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                                        if (pixel + threshold < comp_pixel) {
                                                                            GENERATE_NMS_VALS();
                                                                        }
                                                                    }
                                                                }
                                                                else {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                                        if (pixel + threshold < comp_pixel) {
                                                                            GENERATE_NMS_VALS();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row) * width + col - 3];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel + threshold < comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                            else {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 1) * width + col - 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row + 1) * width + col - 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col - 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 3) * width + col];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row + 3) * width + col + 1];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        pixel = raw_data[(row + 2) * width + col + 2];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col - 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 2) * width + col - 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel - threshold > comp_pixel) {
                    pixel = raw_data[(row) * width + col - 3];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row + 3) * width + col];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col - 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row - 2) * width + col - 2];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col + 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col + 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        pixel = raw_data[(row - 1) * width + col + 3];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col - 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    pixel = raw_data[(row + 3) * width + col];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row) * width + col - 3];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col - 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row - 2) * width + col - 2];
                        if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 1) * width + col - 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (pixel - threshold > comp_pixel) {
                pixel = raw_data[(row - 3) * width + col];
                if (pixel + threshold < comp_pixel) {
                    pixel = raw_data[(row) * width + col - 3];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row + 2) * width + col - 2];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row - 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row - 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row - 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row + 1) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row - 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row - 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row + 3) * width + col];
                        if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col - 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col + 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 1) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col + 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col - 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        pixel = raw_data[(row - 1) * width + col + 3];
                        if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col - 1];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel - threshold > comp_pixel) {
                    pixel = raw_data[(row - 1) * width + col - 3];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row + 2) * width + col + 2];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 1) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col - 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 3) * width + col - 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col - 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row - 1) * width + col + 3];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 1) * width + col - 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else if (pixel + threshold >= comp_pixel && pixel - threshold <= comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col + 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col + 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 1) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel + threshold < comp_pixel) {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                                else if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                                else {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                                        if (pixel - threshold > comp_pixel) {
                                                                            GENERATE_NMS_VALS();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                GENERATE_NMS_VALS();
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                                else {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                                        if (pixel - threshold > comp_pixel) {
                                                                            GENERATE_NMS_VALS();
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row) * width + col - 3];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                                    if (pixel - threshold > comp_pixel) {
                                                                        GENERATE_NMS_VALS();
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                    else if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col + 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel || pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                            else {
                                                                pixel = raw_data[(row) * width + col - 3];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 3) * width + col];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                        else {
                                                            pixel = raw_data[(row) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row + 3) * width + col];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row + 1) * width + col - 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                        else if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row - 2) * width + col + 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                                if (pixel - threshold > comp_pixel) {
                                                                    GENERATE_NMS_VALS();
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                        else {
                                                            pixel = raw_data[(row + 3) * width + col - 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        pixel = raw_data[(row + 2) * width + col + 2];
                        if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row) * width + col + 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 2) * width + col - 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row) * width + col - 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    pixel = raw_data[(row + 3) * width + col];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row - 2) * width + col - 2];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 1) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row) * width + col - 3];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 1) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row + 1) * width + col - 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 2) * width + col - 2];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row - 1) * width + col + 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row) * width + col + 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 2) * width + col - 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                pixel = raw_data[(row) * width + col - 3];
                if (pixel + threshold < comp_pixel) {
                    pixel = raw_data[(row - 2) * width + col - 2];
                    if (pixel + threshold < comp_pixel) {
                        pixel = raw_data[(row - 3) * width + col];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 2) * width + col - 2];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row - 1) * width + col - 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col + 1];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel + threshold < comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row) * width + col + 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                pixel = raw_data[(row - 1) * width + col + 3];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col - 3];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel + threshold < comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 1) * width + col - 3];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 2) * width + col - 2];
                                                    if (pixel + threshold < comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel + threshold < comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col];
                                                if (pixel + threshold < comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel + threshold < comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel + threshold < comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (pixel - threshold > comp_pixel) {
                    pixel = raw_data[(row - 2) * width + col - 2];
                    if (pixel - threshold > comp_pixel) {
                        pixel = raw_data[(row - 3) * width + col];
                        if (pixel + threshold < comp_pixel) {
                            pixel = raw_data[(row + 3) * width + col + 1];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 3) * width + col - 1];
                                if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col + 2];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 3) * width + col];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 2) * width + col - 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel - threshold > comp_pixel) {
                            pixel = raw_data[(row - 1) * width + col - 3];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col - 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row) * width + col + 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 2) * width + col + 2];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 3) * width + col];
                                    if (pixel + threshold < comp_pixel) {
                                        pixel = raw_data[(row - 2) * width + col + 2];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col + 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row + 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col + 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row + 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    GENERATE_NMS_VALS();
                                                }
                                                else {
                                                    pixel = raw_data[(row - 2) * width + col + 2];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                pixel = raw_data[(row + 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 1) * width + col + 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 3) * width + col + 1];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        pixel = raw_data[(row - 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 3) * width + col - 1];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                    else {
                                                        pixel = raw_data[(row) * width + col + 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            pixel = raw_data[(row - 1) * width + col + 3];
                                                            if (pixel - threshold > comp_pixel) {
                                                                GENERATE_NMS_VALS();
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 1) * width + col + 3];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 1) * width + col - 3];
                                        if (pixel + threshold < comp_pixel) {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row - 2) * width + col + 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 3) * width + col - 1];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            pixel = raw_data[(row) * width + col + 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 2) * width + col + 2];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row - 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row - 3) * width + col + 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            pixel = raw_data[(row + 3) * width + col];
                            if (pixel - threshold > comp_pixel) {
                                pixel = raw_data[(row + 2) * width + col + 2];
                                if (pixel + threshold < comp_pixel) {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 1) * width + col - 3];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 3) * width + col - 1];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel - threshold > comp_pixel) {
                                    pixel = raw_data[(row + 2) * width + col - 2];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col - 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 1) * width + col - 3];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col + 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        GENERATE_NMS_VALS();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    pixel = raw_data[(row - 3) * width + col - 1];
                                    if (pixel - threshold > comp_pixel) {
                                        pixel = raw_data[(row + 3) * width + col + 1];
                                        if (pixel - threshold > comp_pixel) {
                                            pixel = raw_data[(row + 2) * width + col - 2];
                                            if (pixel - threshold > comp_pixel) {
                                                pixel = raw_data[(row - 1) * width + col - 3];
                                                if (pixel - threshold > comp_pixel) {
                                                    pixel = raw_data[(row + 3) * width + col - 1];
                                                    if (pixel - threshold > comp_pixel) {
                                                        pixel = raw_data[(row + 1) * width + col - 3];
                                                        if (pixel - threshold > comp_pixel) {
                                                            GENERATE_NMS_VALS();
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // #pragma omp critical
        // intermediate.insert(intermediate.end(), localPoints.begin(), localPoints.end());
    }

    // for (size_t i = 0; i < intermediate.size(); i++) {
    //     bool add = true;
    //     for (size_t j = 0; j < intermediate.size(); j++) {
    //         const auto& p1 = intermediate[i];
    //         const auto& p2 = intermediate[j];
    //         if (cv::KeyPoint::adj(p1, p2) && intermediate_scores[i] < intermediate_scores[j]) add = false;
    //     }
    //     if (add) keypoints.push_back(intermediate[i]);
    // }

    keypoints.reserve(prod / 9 / 1000); // kinda random guess about how many keypoints there will be

    for (int i = 0; i < prod; i++) {
        if ((!nonMaxSuppression && nmsVals[i] != 0) || (nmsVals[i] != 0 && nmsVals[i] > nmsVals[i - width] && nmsVals[i] > nmsVals[i + width] && nmsVals[i] > nmsVals[i - 1] && nmsVals[i] > nmsVals[i + 1] &&
            nmsVals[i] > nmsVals[i - width - 1] && nmsVals[i] > nmsVals[i - width + 1] && nmsVals[i] > nmsVals[i + width - 1] && nmsVals[i] > nmsVals[i + width + 1])) {
            keypoints.push_back(cv::KeyPoint(i % width, i / height, 7));
        }
    }
}


int main() {
    cv::Mat src = cv::imread("train_images/libe.png", cv::IMREAD_GRAYSCALE);
    // cv::Mat src;
    // cv::resize(read, src, cv::Size(640, 480));
    std::vector<cv::KeyPoint> points;
    auto start_time = std::chrono::system_clock::now();
    learnedFast(points, src, 10, false);
    auto end_time = std::chrono::system_clock::now();
    cout << "Homemade Gaussian Blur took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    std::vector<cv::KeyPoint> cvPoints;
    start_time = std::chrono::system_clock::now();
    cv::FAST(src, cvPoints, 10, false);
    end_time = std::chrono::system_clock::now();
    cout << "OpenCV Gaussian Blur took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;


    cout << points.size() << " " << cvPoints.size() << endl;
    // assert(points.size() == cvPoints.size());
    for (size_t i = 0; i < min(points.size(), cvPoints.size()); i++) {
        bool matched = false;
        for (size_t j = 0; j < cvPoints.size(); j++) {
            if (points[i].pt == points[j].pt) matched = true;
        }
        assert(matched);
    }

    return 0;
}