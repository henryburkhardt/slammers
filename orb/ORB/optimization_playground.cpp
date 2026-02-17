#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "src/harris.hpp"
// #include "src/processing.hpp"

using namespace std;

struct CompareKeyPoint {
    bool operator()(const cv::KeyPoint& kp1, const cv::KeyPoint& kp2) {
        return kp1.response > kp2.response; // Min-heap based on response
    }
};

inline void getSurroundingPoints(const uchar* ptrs[7], const int col, uchar* surrPixels) {
    surrPixels[0] = ptrs[0][col - 1];
    surrPixels[1] = ptrs[0][col];
    surrPixels[2] = ptrs[0][col + 1];
    surrPixels[3] = ptrs[1][col + 2];
    surrPixels[4] = ptrs[2][col + 3];
    surrPixels[5] = ptrs[3][col + 3];
    surrPixels[6] = ptrs[4][col + 3];
    surrPixels[7] = ptrs[5][col + 2];
    surrPixels[8] = ptrs[6][col + 1];
    surrPixels[9] = ptrs[6][col];
    surrPixels[10] = ptrs[6][col - 1];
    surrPixels[11] = ptrs[5][col - 2];
    surrPixels[12] = ptrs[4][col - 3];
    surrPixels[13] = ptrs[3][col - 3];
    surrPixels[14] = ptrs[2][col - 3];
    surrPixels[15] = ptrs[1][col - 2];
    surrPixels[16] = ptrs[3][col];
}

int get_corner_score(const uchar* ptrs[7], const int col) {
    uchar surrPixels[17];
    getSurroundingPoints(ptrs, col, surrPixels);
    int center = surrPixels[16];

    int diffs[16];
    for (int i = 0; i < 16; i++) {
        diffs[i] = surrPixels[i] - center;
    }

    int best = 0;
    for (int i = 0; i < 16; i++) {
        int max_diff = INT_MIN;
        int min_diff = INT_MAX;
        for (int j = 0; j < 9; j++) {
            int diff = diffs[(i + j) & 15];
            if (diff > max_diff) max_diff = diff;
            if (min_diff > diff) min_diff = diff;
        }
        if (-max_diff > best) best = -max_diff;
        if (min_diff > best) best = min_diff;
    }
    return best - 1;
}

void learnedFast(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int threshold, bool nonMaxSuppression = true) {
    int height = img.size().height;
    int width = img.size().width;
    int prod = height * width;

    std::vector<uchar> nmsVals(prod, 0);

    #pragma omp parallel for
    for (int row = 3; row < height - 3; row++) {
        const uchar* ptr_arr[7] = {
            img.ptr<uchar>(row - 3),
            img.ptr<uchar>(row - 2),
            img.ptr<uchar>(row - 1),
            img.ptr<uchar>(row),
            img.ptr<uchar>(row + 1),
            img.ptr<uchar>(row + 2),
            img.ptr<uchar>(row + 3)
        };

        for (int col = 3; col < width - 3; col++) {
            uchar pixel = ptr_arr[3][col];
            if (pixel - threshold > ptr_arr[3][col - 3]) {
                if (pixel - threshold > ptr_arr[6][col]) {
                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                    else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                    else {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                    else {
                                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                    if (pixel - threshold > ptr_arr[0][col]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                            else {
                                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                            else {
                                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                            else {
                                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    else if (pixel + threshold < ptr_arr[3][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col]) {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[0][col]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                else if (pixel + threshold < ptr_arr[6][col]) {
                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                        if (pixel - threshold > ptr_arr[0][col]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[0][col]) {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    else if (pixel + threshold < ptr_arr[1][col + 2]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[6][col - 1]) {
                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    if (pixel - threshold > ptr_arr[0][col]) {
                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    else if (pixel + threshold < ptr_arr[0][col]) {
                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
            else if (pixel + threshold < ptr_arr[3][col - 3]) {
                if (pixel - threshold > ptr_arr[0][col]) {
                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        else if (pixel + threshold < ptr_arr[2][col + 3]) {
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    else if (pixel + threshold < ptr_arr[5][col + 2]) {
                        if (pixel - threshold > ptr_arr[6][col]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[6][col]) {
                            if (pixel - threshold > ptr_arr[3][col + 3]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[3][col + 3]) {
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[3][col + 3]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[2][col - 3]) {
                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                else if (pixel + threshold < ptr_arr[0][col]) {
                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                        if (pixel - threshold > ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                if (pixel - threshold > ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[1][col + 2]) {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[6][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[6][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[5][col - 2]) {
                            if (pixel - threshold > ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[6][col]) {
                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                        if (pixel - threshold > ptr_arr[6][col - 1]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[6][col]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (pixel + threshold < ptr_arr[3][col + 3]) {
                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[1][col - 2]) {
                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                        else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                    if (pixel - threshold > ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                                    if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                        else {
                                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[6][col - 1]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[4][col + 3]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                        if (pixel + threshold < ptr_arr[6][col]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    if (pixel - threshold > ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[4][col + 3]) {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[3][col + 3]) {
                                                    if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    else if (pixel + threshold < ptr_arr[6][col]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[6][col - 1]) {
                                if (pixel - threshold > ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                            if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[2][col + 3]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                        if (pixel + threshold < ptr_arr[3][col + 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[1][col - 2]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[2][col - 3]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                                else {
                                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                        if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                if (pixel - threshold > ptr_arr[3][col + 3]) {
                    if (pixel - threshold > ptr_arr[6][col]) {
                        if (pixel - threshold > ptr_arr[2][col + 3]) {
                            if (pixel - threshold > ptr_arr[4][col + 3]) {
                                if (pixel - threshold > ptr_arr[0][col + 1]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                                else if (pixel + threshold < ptr_arr[6][col + 1]) {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                                else {
                                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                        if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                    else {
                                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[4][col - 3]) {
                                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[2][col - 3]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[1][col - 2]) {
                                            if (pixel - threshold > ptr_arr[0][col - 1]) {
                                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[0][col]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel - threshold > ptr_arr[5][col + 2]) {
                                        if (pixel - threshold > ptr_arr[4][col - 3]) {
                                            if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                        else {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[6][col - 1]) {
                                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                else if (pixel + threshold < ptr_arr[6][col]) {
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                    else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[5][col + 2]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[0][col]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[1][col - 2]) {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[1][col + 2]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel - threshold > ptr_arr[6][col + 1]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                    if (pixel - threshold > ptr_arr[0][col - 1]) {
                        if (pixel - threshold > ptr_arr[4][col + 3]) {
                            if (pixel - threshold > ptr_arr[0][col + 1]) {
                                if (pixel - threshold > ptr_arr[1][col + 2]) {
                                    if (pixel - threshold > ptr_arr[1][col - 2]) {
                                        if (pixel - threshold > ptr_arr[0][col]) {
                                            if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                            else {
                                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                                            if (pixel - threshold > ptr_arr[0][col]) {
                                                if (pixel - threshold > ptr_arr[5][col + 2]) {
                                                    if (pixel - threshold > ptr_arr[2][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
            else if (pixel + threshold < ptr_arr[3][col + 3]) {
                if (pixel - threshold > ptr_arr[0][col]) {
                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                        if (pixel + threshold < ptr_arr[2][col + 3]) {
                            if (pixel + threshold < ptr_arr[6][col + 1]) {
                                if (pixel - threshold > ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[5][col - 2]) {
                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                    if (pixel + threshold < ptr_arr[4][col - 3]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[6][col]) {
                                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
            else if (pixel + threshold < ptr_arr[0][col]) {
                if (pixel + threshold < ptr_arr[4][col + 3]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel - threshold > ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                if (pixel - threshold > ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (pixel + threshold < ptr_arr[2][col - 3]) {
                                    if (pixel + threshold < ptr_arr[1][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[5][col + 2]) {
                                        if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel - threshold > ptr_arr[1][col + 2]) {
                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                        if (pixel + threshold < ptr_arr[5][col - 2]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (pixel + threshold < ptr_arr[1][col + 2]) {
                                if (pixel + threshold < ptr_arr[5][col + 2]) {
                                    if (pixel + threshold < ptr_arr[0][col - 1]) {
                                        if (pixel - threshold > ptr_arr[0][col + 1]) {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                        else if (pixel + threshold < ptr_arr[0][col + 1]) {
                                            nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                    if (pixel + threshold < ptr_arr[6][col]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                            else {
                                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                                    if (pixel + threshold < ptr_arr[6][col - 1]) {
                                                        nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel + threshold < ptr_arr[1][col - 2]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {
                                if (pixel + threshold < ptr_arr[4][col - 3]) {
                                    if (pixel + threshold < ptr_arr[5][col - 2]) {
                                        if (pixel + threshold < ptr_arr[6][col - 1]) {
                                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                                if (pixel + threshold < ptr_arr[6][col]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if (pixel + threshold < ptr_arr[1][col - 2]) {
                                if (pixel + threshold < ptr_arr[1][col + 2]) {
                                    if (pixel + threshold < ptr_arr[2][col - 3]) {
                                        if (pixel + threshold < ptr_arr[0][col - 1]) {
                                            if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                    }
                                    else {
                                        if (pixel + threshold < ptr_arr[5][col + 2]) {
                                            if (pixel + threshold < ptr_arr[0][col - 1]) {
                                                if (pixel + threshold < ptr_arr[0][col + 1]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
                if (pixel + threshold < ptr_arr[6][col - 1]) {
                    if (pixel + threshold < ptr_arr[2][col + 3]) {
                        if (pixel + threshold < ptr_arr[6][col + 1]) {
                            if (pixel + threshold < ptr_arr[5][col + 2]) {
                                if (pixel + threshold < ptr_arr[5][col - 2]) {
                                    if (pixel + threshold < ptr_arr[6][col]) {
                                        if (pixel + threshold < ptr_arr[1][col + 2]) {
                                            if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                            }
                                        }
                                        else {
                                            if (pixel + threshold < ptr_arr[4][col - 3]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    if (pixel + threshold < ptr_arr[0][col + 1]) {
                                        if (pixel + threshold < ptr_arr[6][col]) {
                                            if (pixel + threshold < ptr_arr[1][col + 2]) {
                                                if (pixel + threshold < ptr_arr[4][col + 3]) {
                                                    nmsVals[row * width + col] = get_corner_score(ptr_arr, col);
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
    }

    uchar* prev_pointer = &nmsVals[2 * width];
    uchar* curr_pointer = &nmsVals[3 * width];
    uchar* next_pointer = &nmsVals[4 * width];

    for (int row = 3; row < height - 3; row++) {
        for (int col = 3; col < width - 3; col++) {
            uchar val = curr_pointer[col];
            if (!nonMaxSuppression && val != 0 || val > prev_pointer[col - 1] && val > prev_pointer[col] && val > prev_pointer[col + 1] &&
                                                  val > curr_pointer[col - 1] && val > curr_pointer[col + 1] &&
                                                  val > next_pointer[col - 1] && val > next_pointer[col] && val > next_pointer[col + 1])
                keypoints.emplace_back(col, row, 7, -1.0F, val);
        }
        prev_pointer = curr_pointer;
        curr_pointer = next_pointer;
        next_pointer = &nmsVals[(row + 2) * width];
    }
}


const int CENTROID_RADIUS = 15;

static float calculateOrientation(const cv::Mat& image, cv::Point2f pt,  const vector<int> & u_max)
{
    int step = image.step;
    unsigned char* raw_data = image.data;
    int row = pt.y;
    int col = pt.x;
    unsigned char* center = &raw_data[row * step + col];
    int moment10 = 0;
    int moment01 = 0;
    for (int i = 0; i <= CENTROID_RADIUS; i++) {
        for (int j = -u_max[i]; j <= u_max[i]; j++) {
            uchar top_intensity = i != 0 ? center[-step * i + j] : 0;
            uchar bottom_intensity = center[step * i + j];
            moment10 += j * (bottom_intensity + top_intensity);
            moment01 += i * (bottom_intensity - top_intensity);
        }
    }

    return cv::fastAtan2((float)moment01, (float)moment10);
}


int main() {
    cv::Mat read = cv::imread("train_images/libe.png", cv::IMREAD_GRAYSCALE);
    cv::Mat src;
    cv::resize(read, src, cv::Size(640, 480));
    std::vector<cv::KeyPoint> points;
    auto start_time = std::chrono::system_clock::now();
    learnedFast(src, points, 10, true);
    auto end_time = std::chrono::system_clock::now();
    cout << "Homemade FAST took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    std::vector<cv::KeyPoint> cvPoints;
    start_time = std::chrono::system_clock::now();
    cv::FAST(src, cvPoints, 10, true);
    end_time = std::chrono::system_clock::now();
    cout << "OpenCV FAST took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    cout << points.size() << " " << cvPoints.size() << endl;

    for (const auto& point : points) {
        for (const auto& cvPoint: cvPoints) {
            if (point.pt == cvPoint.pt && point.response != cvPoint.response) cout << point.response << " " << cvPoint.response << endl;
        }
    }

    return 0;
}
