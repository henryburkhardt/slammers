#include "CV.h"

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

void learnedFast(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int threshold, bool nonMaxSuppression) {
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
