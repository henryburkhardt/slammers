#include <opencv2/opencv.hpp>

void blurImage(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev) {
    // check that input values are valid
    CV_Assert(kernelRadius >= 0);
    CV_Assert(stdDev > 0);

    int width = src.size().width; int height = src.size().height;

    std::vector<double> kernel(2 * kernelRadius + 1);
    double stdDevSquared = stdDev * stdDev;

    // caluclate normalized gaussian kernel
    double sum = 0;
    for (int i = -kernelRadius; i <= kernelRadius; i++) {
        double val = 1.0 / sqrt(2 * M_PI * stdDevSquared) * exp(- i * i / (2 * stdDevSquared));
        kernel[i + kernelRadius] =  val;
        sum += val;
    }

    for (double& val : kernel) {
        val /= sum;
    }

    // initialize other used values
    dest.create(src.size(), src.type());
    cv::Mat temp(height, width, CV_64F);

    const uchar *src_data = src.ptr<uchar>();
    double *temp_data = temp.ptr<double>();
    uchar *dest_data = dest.ptr<uchar>();

    // perform horizontal filter application over image to temp.
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = kernelRadius; i < height - kernelRadius; i++) {
            for (int j = kernelRadius; j < width - kernelRadius; j++) {
                double pixelValue = 0.0;
                int pixel_index = i * width + j;
                const uchar *kernel_ptr = &src_data[pixel_index - kernelRadius];
                for (int k = 0; k <= 2 * kernelRadius; k++) {
                    pixelValue += kernel_ptr[k] * kernel[k];
                }
                // set temp image to computed value and increment index for next pixel
                temp_data[pixel_index] = pixelValue;
            }
        }

        // perform vertical pass kernel application from temp to dest.
        #pragma omp for
        for (int i = kernelRadius; i < height - kernelRadius; i++) {
            for (int j = kernelRadius; j < width - kernelRadius; j++) {
                double pixelValue = 0.0;
                int pixel_index = i * width + j;
                for (int k = -kernelRadius; k <= kernelRadius; k++) {
                    pixelValue += temp_data[pixel_index + k * width] * kernel[k + kernelRadius];
                }
                // set temp image to computed value and increment index for next pixel
                dest_data[pixel_index] = cvRound(pixelValue);
            }
        }
    }
}

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

void calculateOrientation(const cv::Mat& img, cv::KeyPoint& point) {
    int height = img.size().height;
    int width = img.size().width;
    int step = img.step;
    unsigned char* raw_data = img.data;
    int row = point.pt.y;
    int col = point.pt.x;
    int moment10 = 0;
    int moment01 = 0;
    for (int i = -CENTROID_RADIUS; i <= CENTROID_RADIUS; i++) {
        for (int j = -CENTROID_RADIUS; j <= CENTROID_RADIUS; j++) {
            int y = row + i;
            int x = col + j;
            if (i * i + j * j > CENTROID_RADIUS * CENTROID_RADIUS || x < 0 || x >= width || y < 0 || y >= height) continue;
            uchar intensity = raw_data[y * step + x];
            moment10 += j * intensity;
            moment01 += i * intensity;
        }
    }
    double angle = atan2(moment01, moment10) * 180 / M_PI;
    if (angle < 0) angle += 360;
    point.angle = angle;
}