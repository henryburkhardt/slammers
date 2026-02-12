#include "CV.h"

void blur(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev) {
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

void learnedFast(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int threshold, bool nonMaxSuppression) {
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

    // keypoints.reserve(prod / 9 / 1000); // kinda random guess about how many keypoints there will be

    for (int i = 0; i < prod; i++) {
        if (nmsVals[i] != 0 && (!nonMaxSuppression || (nmsVals[i] > nmsVals[i - width] && nmsVals[i] > nmsVals[i + width] && nmsVals[i] > nmsVals[i - 1] && nmsVals[i] > nmsVals[i + 1] &&
            nmsVals[i] > nmsVals[i - width - 1] && nmsVals[i] > nmsVals[i - width + 1] && nmsVals[i] > nmsVals[i + width - 1] && nmsVals[i] > nmsVals[i + width + 1]))) {
            keypoints.push_back(cv::KeyPoint(i % width, i / width, 7, nmsVals[i]));
        }
    }
    free(nmsVals);
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