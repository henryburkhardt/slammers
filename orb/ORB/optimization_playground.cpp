#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "src/fast.hpp"

using namespace std;

std::array<int, 17> getSurroundingPoints(const cv::Mat& img, int row, int col) {
    uchar* raw_data = img.data;
    
    int width = img.step;
    int center_point = row * width + col;

    return {
        raw_data[center_point - 3 * width - 1],
        raw_data[center_point - 3 * width],
        raw_data[center_point - 3 * width + 1],
        raw_data[center_point - 2 * width + 2],
        raw_data[center_point - width + 3],
        raw_data[center_point + 3],
        raw_data[center_point + width + 3],
        raw_data[center_point + 2 * width + 2],
        raw_data[center_point + 3 * width + 1],
        raw_data[center_point + 3 * width],
        raw_data[center_point + 3 * width - 1],
        raw_data[center_point + 2 * width - 2],
        raw_data[center_point + width - 3],
        raw_data[center_point - 3],
        raw_data[center_point - width - 3],
        raw_data[center_point - 2 * width - 2],
        raw_data[center_point]
    };
}

int get_corner_score(const std::array<int, 17>& pointVector) {
    int center = pointVector[16];

    int ring[32];
    for(int i = 0; i < 16; i++) {
        ring[i] = ring[i + 16] = pointVector[i];
    }

    int best = 0;
    for (int i = 0; i < 16; i++) {
        int max_diff = INT_MIN;
        int min_diff = INT_MAX;
        for (int j = 0; j < 9; j++) {
            int diff = ring[i + j] - center;
            max_diff = std::min(0, std::max(max_diff, diff));
            min_diff = std::max(0, std::min(min_diff, diff));
        }
        best = std::max(best, -max_diff);
        best = std::max(best, min_diff);
    }
    return best - 1;
}

void learnedFast(std::vector<cv::KeyPoint>& keypoints, const cv::Mat& img, int threshold, bool nonMaxSuppression = true) {
    int height = img.size().height;
    int width = img.size().width;
    int prod = height * width;
    uint *nmsVals = (uint *) malloc(sizeof(uint) * prod);
    for (int i = 0; i < prod; i++) nmsVals[i] = 0;
        
    #pragma omp parallel for
    for (int row = 3; row < height - 3; row++) {
        for (int col = 3; col < width - 3; col++) {
            const auto surrPoints = getSurroundingPoints(img, row, col);
            int pixel = surrPoints[16];
            // Paste python output code starting here.
            if (pixel - threshold > surrPoints[6]) {
                if (pixel - threshold > surrPoints[1]) {
                    if (pixel - threshold > surrPoints[14]) {
                        if (pixel - threshold > surrPoints[4]) {
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel + threshold < surrPoints[13] || pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                                else {
                                                                    if (pixel - threshold > surrPoints[7]) {
                                                                        if (pixel - threshold > surrPoints[8]) {
                                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel + threshold < surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    if (pixel - threshold > surrPoints[7]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel + threshold < surrPoints[12]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                                else if (pixel + threshold < surrPoints[0]) {
                                                                    if (pixel - threshold > surrPoints[7]) {
                                                                        if (pixel - threshold > surrPoints[8]) {
                                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                        }
                                                                    }
                                                                }
                                                                else {
                                                                    if (pixel - threshold > surrPoints[8]) {
                                                                        if (pixel - threshold > surrPoints[7]) {
                                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    if (pixel - threshold > surrPoints[8]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    if (pixel - threshold > surrPoints[8]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else if (pixel + threshold < surrPoints[8]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    if (pixel - threshold > surrPoints[13]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    if (pixel - threshold > surrPoints[0]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel + threshold < surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[13]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                if (pixel - threshold > surrPoints[11]) {
                                                                    if (pixel - threshold > surrPoints[9]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[11]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else if (pixel + threshold < surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold >= surrPoints[8] && pixel - threshold <= surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else if (pixel + threshold >= surrPoints[2] && pixel - threshold <= surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else if (pixel + threshold < surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[2]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[13]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    if (pixel - threshold > surrPoints[9]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                if (pixel - threshold > surrPoints[10]) {
                                                                    if (pixel - threshold > surrPoints[9]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else if (pixel + threshold >= surrPoints[0] && pixel - threshold <= surrPoints[0]) {
                                                                if (pixel + threshold < surrPoints[3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[0] || pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[8]) {
                                                                    if (pixel - threshold > surrPoints[7]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            else if (pixel + threshold < surrPoints[15]) {
                                if (pixel - threshold > surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[13]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                if (pixel - threshold > surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[13]) {
                                                            if (pixel - threshold > surrPoints[12]) {
                                                                if (pixel - threshold > surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                            else {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[5]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel - threshold > surrPoints[8]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[13]) {
                                                            if (pixel - threshold > surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                if (pixel - threshold > surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[13] || pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[4]) {
                            if (pixel - threshold > surrPoints[12]) {
                                if (pixel - threshold > surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[13]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[15]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else if (pixel + threshold >= surrPoints[9] && pixel - threshold <= surrPoints[9]) {
                                                                if (pixel - threshold > surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[9] || pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[12]) {
                                if (pixel - threshold > surrPoints[13]) {
                                    if (pixel - threshold > surrPoints[11]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                if (pixel - threshold > surrPoints[15]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[15]) {
                                                                if (pixel - threshold > surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[2]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel - threshold > surrPoints[9]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[14]) {
                        if (pixel - threshold > surrPoints[8]) {
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel - threshold > surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[5]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[4]) {
                                if (pixel - threshold > surrPoints[13]) {
                                    if (pixel - threshold > surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[13]) {
                                    if (pixel - threshold > surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[8]) {
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel - threshold > surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[15]) {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel + threshold < surrPoints[0]) {
                                        if (pixel + threshold < surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[7]) {
                                    if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[13]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[0]) {
                                        if (pixel + threshold < surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[10]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel - threshold > surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        if (pixel - threshold > surrPoints[7]) {
                            if (pixel - threshold > surrPoints[5]) {
                                if (pixel - threshold > surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[13]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[13]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[15]) {
                                        if (pixel - threshold > surrPoints[4]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[15]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[4]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                else if (pixel + threshold < surrPoints[1]) {
                    if (pixel - threshold > surrPoints[13]) {
                        if (pixel - threshold > surrPoints[9]) {
                            if (pixel - threshold > surrPoints[14]) {
                                if (pixel - threshold > surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[4]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[4]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[4]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[14]) {
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[4]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[4]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[4]) {
                                                            if (pixel - threshold > surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[13]) {
                        if (pixel - threshold > surrPoints[15]) {
                            if (pixel - threshold > surrPoints[8]) {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[10]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        else if (pixel + threshold < surrPoints[15]) {
                            if (pixel - threshold > surrPoints[11]) {
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[4]) {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[14]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[5]) {
                                    if (pixel + threshold < surrPoints[14]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[4]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[4]) {
                                            if (pixel + threshold < surrPoints[14]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[11]) {
                                if (pixel - threshold > surrPoints[9]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[3]) {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[9]) {
                                    if (pixel + threshold < surrPoints[14]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[4]) {
                                                            if (pixel + threshold < surrPoints[2]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[2]) {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[5]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[14]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[5]) {
                                            if (pixel + threshold < surrPoints[14]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                if (pixel - threshold > surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else if (pixel + threshold >= surrPoints[14] && pixel - threshold <= surrPoints[14]) {
                                                            if (pixel + threshold >= surrPoints[0] && pixel - threshold <= surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                if (pixel - threshold > surrPoints[7]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        if (pixel - threshold > surrPoints[4]) {
                            if (pixel - threshold > surrPoints[10]) {
                                if (pixel - threshold > surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    if (pixel - threshold > surrPoints[9]) {
                        if (pixel - threshold > surrPoints[13]) {
                            if (pixel - threshold > surrPoints[10]) {
                                if (pixel - threshold > surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[14]) {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[4]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[14]) {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        else if (pixel + threshold < surrPoints[13]) {
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[5]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[14]) {
                                                            if (pixel - threshold > surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                        else if (pixel + threshold < surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[11]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            if (pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[7]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel - threshold > surrPoints[8]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[10]) {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[8]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[9]) {
                        if (pixel + threshold < surrPoints[15]) {
                            if (pixel + threshold < surrPoints[12]) {
                                if (pixel + threshold < surrPoints[8]) {
                                    if (pixel + threshold < surrPoints[10]) {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[13]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
            else if (pixel + threshold < surrPoints[6]) {
                if (pixel - threshold > surrPoints[1]) {
                    if (pixel - threshold > surrPoints[13]) {
                        if (pixel - threshold > surrPoints[11]) {
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel - threshold > surrPoints[9]) {
                                    if (pixel - threshold > surrPoints[14]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[9]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[14]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[2]) {
                                            if (pixel + threshold < surrPoints[10]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[4]) {
                                                        if (pixel + threshold < surrPoints[8]) {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[2]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                if (pixel - threshold > surrPoints[3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                if (pixel - threshold > surrPoints[14]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            else if (pixel + threshold < surrPoints[15]) {
                                if (pixel + threshold < surrPoints[2]) {
                                    if (pixel + threshold < surrPoints[10]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[4]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                                if (pixel + threshold < surrPoints[2]) {
                                    if (pixel + threshold < surrPoints[10]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[4]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        else if (pixel + threshold < surrPoints[11]) {
                            if (pixel - threshold > surrPoints[5]) {
                                if (pixel - threshold > surrPoints[15]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[14]) {
                                            if (pixel - threshold > surrPoints[4]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[5]) {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[15]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[7]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[7]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[12]) {
                                    if (pixel - threshold > surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[15]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[14]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[15]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[14]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[12]) {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[14]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[4]) {
                                if (pixel + threshold < surrPoints[2]) {
                                    if (pixel + threshold < surrPoints[10]) {
                                        if (pixel + threshold < surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[7]) {
                                                if (pixel + threshold < surrPoints[8]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[13]) {
                        if (pixel + threshold < surrPoints[9]) {
                            if (pixel - threshold > surrPoints[14]) {
                                if (pixel + threshold < surrPoints[5]) {
                                    if (pixel + threshold < surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold >= surrPoints[12] && pixel - threshold <= surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[4]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[4]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            if (pixel + threshold < surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[14]) {
                                if (pixel - threshold > surrPoints[11]) {
                                    if (pixel + threshold < surrPoints[2]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[4]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[11]) {
                                    if (pixel + threshold < surrPoints[8]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[15]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        if (pixel + threshold < surrPoints[4]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[4]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[2]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[4]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        if (pixel + threshold < surrPoints[8]) {
                                                            if (pixel + threshold < surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                                if (pixel + threshold < surrPoints[5]) {
                                    if (pixel + threshold < surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[4]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[11]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[8]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        if (pixel + threshold < surrPoints[4]) {
                            if (pixel + threshold < surrPoints[10]) {
                                if (pixel + threshold < surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[7]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[7]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[5]) {
                                            if (pixel + threshold < surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            if (pixel + threshold < surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[7]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[7]) {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                else if (pixel + threshold < surrPoints[1]) {
                    if (pixel - threshold > surrPoints[14]) {
                        if (pixel - threshold > surrPoints[7]) {
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel - threshold > surrPoints[12]) {
                                    if (pixel - threshold > surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[13]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[7]) {
                            if (pixel - threshold > surrPoints[4]) {
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[0]) {
                                        if (pixel - threshold > surrPoints[8]) {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[15]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[13]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[15]) {
                                                            if (pixel - threshold > surrPoints[12]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[13]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > surrPoints[0]) {
                                        if (pixel - threshold > surrPoints[8]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[13]) {
                                                            if (pixel - threshold > surrPoints[11]) {
                                                                if (pixel - threshold > surrPoints[15]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            else if (pixel + threshold < surrPoints[4]) {
                                if (pixel - threshold > surrPoints[9]) {
                                    if (pixel - threshold > surrPoints[15]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[0]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[15]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[9]) {
                                    if (pixel + threshold < surrPoints[5]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[0]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[15]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[13]) {
                                    if (pixel - threshold > surrPoints[0]) {
                                        if (pixel - threshold > surrPoints[8]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[15]) {
                                                        if (pixel - threshold > surrPoints[9]) {
                                                            if (pixel - threshold > surrPoints[12]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[13]) {
                                    if (pixel + threshold < surrPoints[5]) {
                                        if (pixel + threshold < surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[0]) {
                                if (pixel - threshold > surrPoints[8]) {
                                    if (pixel - threshold > surrPoints[11]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[15]) {
                                                if (pixel - threshold > surrPoints[12]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[13]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[14]) {
                        if (pixel - threshold > surrPoints[4]) {
                            if (pixel + threshold < surrPoints[12]) {
                                if (pixel + threshold < surrPoints[11]) {
                                    if (pixel + threshold < surrPoints[13]) {
                                        if (pixel - threshold > surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[15]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[10]) {
                                            if (pixel - threshold > surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[8]) {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[15]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            if (pixel + threshold < surrPoints[15]) {
                                                                if (pixel + threshold < surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[15]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else if (pixel + threshold >= surrPoints[9] && pixel - threshold <= surrPoints[9]) {
                                                                if (pixel + threshold < surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[4]) {
                            if (pixel - threshold > surrPoints[15]) {
                                if (pixel + threshold < surrPoints[8]) {
                                    if (pixel + threshold < surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[9]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[13] || pixel - threshold > surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[11]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                if (pixel + threshold < surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[5]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[15]) {
                                if (pixel - threshold > surrPoints[5]) {
                                    if (pixel + threshold < surrPoints[12]) {
                                        if (pixel + threshold < surrPoints[13]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[8] || pixel - threshold > surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[3]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[5]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[11]) {
                                                                if (pixel + threshold < surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[7]) {
                                                                    if (pixel + threshold < surrPoints[8]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[7]) {
                                                                if (pixel + threshold < surrPoints[8]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel + threshold < surrPoints[11]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                if (pixel + threshold < surrPoints[11]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[9]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel - threshold > surrPoints[12]) {
                                                                if (pixel + threshold < surrPoints[7]) {
                                                                    if (pixel + threshold < surrPoints[8]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel + threshold < surrPoints[12]) {
                                                                if (pixel - threshold > surrPoints[13]) {
                                                                    if (pixel + threshold < surrPoints[7]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                                else if (pixel + threshold < surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                                else {
                                                                    if (pixel + threshold < surrPoints[7]) {
                                                                        if (pixel + threshold < surrPoints[8]) {
                                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[8]) {
                                                                    if (pixel + threshold < surrPoints[7]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[2]) {
                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[9]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel + threshold < surrPoints[12]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                                else {
                                                                    if (pixel + threshold < surrPoints[8]) {
                                                                        if (pixel + threshold < surrPoints[7]) {
                                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[7]) {
                                                                    if (pixel + threshold < surrPoints[8]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[11]) {
                                                                if (pixel + threshold < surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                if (pixel + threshold < surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[7]) {
                                                            if (pixel - threshold > surrPoints[8]) {
                                                                if (pixel + threshold < surrPoints[0]) {
                                                                    if (pixel + threshold < surrPoints[13]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                            else if (pixel + threshold < surrPoints[8]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[13]) {
                                                                    if (pixel + threshold < surrPoints[0]) {
                                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel + threshold < surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel + threshold < surrPoints[0]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                                    if (pixel + threshold < surrPoints[13]) {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[8]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[7]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[11]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < surrPoints[3]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[7]) {
                                                                if (pixel + threshold < surrPoints[9]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                                if (pixel + threshold < surrPoints[7]) {
                                    if (pixel + threshold < surrPoints[8]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            if (pixel + threshold < surrPoints[5] || pixel - threshold > surrPoints[5]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                            else {
                                                                if (pixel + threshold < surrPoints[13]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[9]) {
                                                if (pixel - threshold > surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            if (pixel + threshold < surrPoints[13]) {
                                                                if (pixel + threshold < surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            if (pixel + threshold < surrPoints[12]) {
                                                                if (pixel + threshold < surrPoints[10]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[13]) {
                                                            if (pixel + threshold < surrPoints[9]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[5]) {
                                                                if (pixel + threshold < surrPoints[9]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel + threshold < surrPoints[12]) {
                                if (pixel + threshold < surrPoints[11]) {
                                    if (pixel + threshold < surrPoints[13]) {
                                        if (pixel - threshold > surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[15]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[7]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[15]) {
                                                                if (pixel + threshold < surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[15]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[15]) {
                                                            if (pixel + threshold < surrPoints[0]) {
                                                                if (pixel + threshold < surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[10]) {
                                                            if (pixel + threshold < surrPoints[3]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                        else if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[3]) {
                                                                if (pixel + threshold < surrPoints[2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                        else {
                                                            if (pixel + threshold < surrPoints[10]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                        if (pixel + threshold < surrPoints[7]) {
                            if (pixel + threshold < surrPoints[5]) {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel + threshold < surrPoints[13]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[9]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[4]) {
                                    if (pixel + threshold < surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[15]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[15]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[13]) {
                                        if (pixel + threshold < surrPoints[9]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    if (pixel - threshold > surrPoints[9]) {
                        if (pixel - threshold > surrPoints[15]) {
                            if (pixel - threshold > surrPoints[8]) {
                                if (pixel - threshold > surrPoints[12]) {
                                    if (pixel - threshold > surrPoints[0]) {
                                        if (pixel - threshold > surrPoints[14]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[13]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[0]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[13]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[13]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[14]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                    else if (pixel + threshold < surrPoints[9]) {
                        if (pixel - threshold > surrPoints[13]) {
                            if (pixel + threshold < surrPoints[4]) {
                                if (pixel + threshold < surrPoints[10]) {
                                    if (pixel + threshold < surrPoints[3]) {
                                        if (pixel + threshold < surrPoints[7]) {
                                            if (pixel + threshold < surrPoints[8]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[5]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[7]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[13]) {
                            if (pixel + threshold < surrPoints[8]) {
                                if (pixel + threshold < surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[5]) {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[7]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[15]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        if (pixel + threshold < surrPoints[15]) {
                                                            if (pixel + threshold < surrPoints[12]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[4]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[4]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[4]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[15]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            if (pixel + threshold < surrPoints[12]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[14]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[12]) {
                                                    if (pixel + threshold < surrPoints[7]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[15]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel + threshold < surrPoints[4]) {
                                if (pixel + threshold < surrPoints[10]) {
                                    if (pixel + threshold < surrPoints[7]) {
                                        if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[8]) {
                                                    if (pixel + threshold < surrPoints[11]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[5]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        if (pixel + threshold < surrPoints[11]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                if (pixel - threshold > surrPoints[13]) {
                    if (pixel - threshold > surrPoints[15]) {
                        if (pixel - threshold > surrPoints[1]) {
                            if (pixel - threshold > surrPoints[11]) {
                                if (pixel - threshold > surrPoints[14]) {
                                    if (pixel - threshold > surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel - threshold > surrPoints[3]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[7]) {
                                                    if (pixel - threshold > surrPoints[10]) {
                                                        if (pixel - threshold > surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[3]) {
                                                    if (pixel - threshold > surrPoints[4]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            if (pixel - threshold > surrPoints[2]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[2]) {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[10]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[2]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel - threshold > surrPoints[5]) {
                                                            if (pixel - threshold > surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > surrPoints[10]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < surrPoints[11]) {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[5]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        if (pixel - threshold > surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[3]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > surrPoints[4]) {
                                    if (pixel - threshold > surrPoints[3]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[0]) {
                                                    if (pixel - threshold > surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[2]) {
                                                    if (pixel - threshold > surrPoints[14]) {
                                                        if (pixel - threshold > surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[5]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[0]) {
                                                        if (pixel - threshold > surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[1]) {
                            if (pixel - threshold > surrPoints[8]) {
                                if (pixel - threshold > surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[14]) {
                                        if (pixel - threshold > surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[12]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[7]) {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[11]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > surrPoints[0]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel - threshold > surrPoints[8]) {
                                if (pixel - threshold > surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel - threshold > surrPoints[14]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[11]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[7]) {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel - threshold > surrPoints[14]) {
                                                    if (pixel - threshold > surrPoints[9]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > surrPoints[0]) {
                                            if (pixel - threshold > surrPoints[11]) {
                                                if (pixel - threshold > surrPoints[9]) {
                                                    if (pixel - threshold > surrPoints[14]) {
                                                        if (pixel - threshold > surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                else if (pixel + threshold < surrPoints[13]) {
                    if (pixel + threshold < surrPoints[15]) {
                        if (pixel - threshold > surrPoints[1]) {
                            if (pixel + threshold < surrPoints[8]) {
                                if (pixel + threshold < surrPoints[10]) {
                                    if (pixel - threshold > surrPoints[7]) {
                                        if (pixel + threshold < surrPoints[0]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[7]) {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[9]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[0]) {
                                            if (pixel + threshold < surrPoints[9]) {
                                                if (pixel + threshold < surrPoints[11]) {
                                                    if (pixel + threshold < surrPoints[14]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < surrPoints[1]) {
                            if (pixel + threshold < surrPoints[14]) {
                                if (pixel - threshold > surrPoints[11]) {
                                    if (pixel + threshold < surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[5]) {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[3]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[5]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[11]) {
                                    if (pixel - threshold > surrPoints[9]) {
                                        if (pixel - threshold > surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[2]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[3]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[10]) {
                                                if (pixel + threshold < surrPoints[2]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < surrPoints[9]) {
                                        if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[10]) {
                                                    nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[3]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < surrPoints[7]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[8]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[4]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            if (pixel + threshold < surrPoints[2]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < surrPoints[2]) {
                                            if (pixel + threshold < surrPoints[0]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                    else {
                                                        if (pixel + threshold < surrPoints[5]) {
                                                            if (pixel + threshold < surrPoints[4]) {
                                                                nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[4]) {
                                        if (pixel - threshold > surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        if (pixel + threshold < surrPoints[0]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < surrPoints[12]) {
                                            if (pixel + threshold < surrPoints[3]) {
                                                if (pixel + threshold < surrPoints[0]) {
                                                    if (pixel + threshold < surrPoints[2]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < surrPoints[5]) {
                                                if (pixel + threshold < surrPoints[3]) {
                                                    if (pixel + threshold < surrPoints[0]) {
                                                        if (pixel + threshold < surrPoints[2]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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
                            if (pixel + threshold < surrPoints[9]) {
                                if (pixel - threshold > surrPoints[7]) {
                                    if (pixel + threshold < surrPoints[0]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[12]) {
                                                        if (pixel + threshold < surrPoints[10]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < surrPoints[7]) {
                                    if (pixel + threshold < surrPoints[11]) {
                                        if (pixel + threshold < surrPoints[10]) {
                                            if (pixel + threshold < surrPoints[12]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[8]) {
                                                        nmsVals[row * width + col] = get_corner_score(surrPoints);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < surrPoints[0]) {
                                        if (pixel + threshold < surrPoints[8]) {
                                            if (pixel + threshold < surrPoints[11]) {
                                                if (pixel + threshold < surrPoints[14]) {
                                                    if (pixel + threshold < surrPoints[10]) {
                                                        if (pixel + threshold < surrPoints[12]) {
                                                            nmsVals[row * width + col] = get_corner_score(surrPoints);
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

            // Stop pasting python output here.
        }
    }

    keypoints.reserve(prod / 9 / 1000); // kinda random guess about how many keypoints there will be

    for (int i = 0; i < prod; i++) {
        if (nmsVals[i] != 0 && (!nonMaxSuppression || (nmsVals[i] > nmsVals[i - width] && nmsVals[i] > nmsVals[i + width] && nmsVals[i] > nmsVals[i - 1] && nmsVals[i] > nmsVals[i + 1] &&
            nmsVals[i] > nmsVals[i - width - 1] && nmsVals[i] > nmsVals[i - width + 1] && nmsVals[i] > nmsVals[i + width - 1] && nmsVals[i] > nmsVals[i + width + 1]))) {
            keypoints.push_back(cv::KeyPoint(i % width, i / width, 7, nmsVals[i]));
        }
    }
}


const int CENTROID_RADIUS = 50;

double calculateOrientation(const cv::Mat& img, cv::Point2i point) {
    int height = img.size().height;
    int width = img.size().width;
    unsigned char* raw_data = img.data;
    int row = point.y;
    int col = point.x;
    int moment10 = 0;
    int moment01 = 0;
    for (int i = 0; i <= 2 * CENTROID_RADIUS; i++) {
        for (int j = 0; j <= 2 * CENTROID_RADIUS; j++) {
            int x = col + j - CENTROID_RADIUS;
            int y = row + i - CENTROID_RADIUS;
            if ((CENTROID_RADIUS - j) * (CENTROID_RADIUS - j) + (CENTROID_RADIUS - i) * (CENTROID_RADIUS - i) > CENTROID_RADIUS * CENTROID_RADIUS || x < 0 || x >= width || y < 0 || y >= height) continue;
            uchar intensity = raw_data[y * width + x];
            moment10 += (i - CENTROID_RADIUS) * intensity;
            moment01 += (j - CENTROID_RADIUS) * intensity;
        }
    }
    return atan2(moment01, moment10);
}


int main() {
    cv::Mat src = cv::imread("train_images/weitz.png", cv::IMREAD_GRAYSCALE);
    // cv::Mat src;
    // cv::resize(read, src, cv::Size(640, 480));
    std::vector<cv::KeyPoint> points;
    auto start_time = std::chrono::system_clock::now();
    learnedFast(points, src, 10, true);
    // speedTest(src);
    auto end_time = std::chrono::system_clock::now();
    cout << "Homemade FAST took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    std::vector<cv::KeyPoint> cvPoints;
    start_time = std::chrono::system_clock::now();
    cv::FAST(src, cvPoints, 10, true);
    end_time = std::chrono::system_clock::now();
    cout << "OpenCV FAST took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    // auto orbDet = cv::ORB::create();
    // cv::Mat descriptors;
    // orbDet->detectAndCompute(src, cv::noArray(), cvPoints, descriptors, false);

    // cout << cvPoints[0].pt << endl;


    cout << points.size() << " " << cvPoints.size() << endl;
    // assert(points.size() == cvPoints.size());
    for (size_t i = 0; i < cvPoints.size(); i++) {
        bool matched = false;
        for (size_t j = 0; j < points.size(); j++) {
            if (points[j].pt == cvPoints[i].pt) {
                matched = true;
                // cout << calculateOrientation(src, points[j].pt) << " " << cvPoints[i].angle << endl;
            }
        }
        if (!matched) {
            cout << cvPoints[i].pt << endl; 
            return 1;
        }
        // assert(matched);
    }

    return 0;
}