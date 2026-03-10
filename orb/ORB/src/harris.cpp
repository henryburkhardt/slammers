#include <opencv2/opencv.hpp>
#include "harris.hpp"

const float RESPONSE_CONSTANT = 0.04f;

void applyKernel(int *xGrad, int *yGrad, const cv::Mat& img) {
    int height = img.size().height;
    int width = img.size().width;
    size_t step = img.step;

    uchar* raw_data = img.data;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int xSum = 0;
            int ySum = 0;
            int centerPixel = i * step + j;
            if (i > 0 && j > 0) {
                int pixelVal = raw_data[centerPixel - step - 1];
                xSum -= pixelVal;
                ySum -= pixelVal;
            }
            if (i > 0) {
                int pixelVal = raw_data[centerPixel - step];
                ySum -= 2 * pixelVal;
            }
            if (i > 0 && j < width - 1) {
                int pixelVal = raw_data[centerPixel - step + 1];
                xSum += pixelVal;
                ySum -= pixelVal;
            }
            if (j > 0) {
                int pixelVal = raw_data[centerPixel - 1];
                xSum -= 2 * pixelVal;
            }
            if (j < width - 1) {
                int pixelVal = raw_data[centerPixel + 1];
                xSum += 2 * pixelVal;
            }
            if (i < height - 1 && j > 0) {
                int pixelVal = raw_data[centerPixel + step - 1];
                xSum -= pixelVal;
                ySum += pixelVal;
            }
            if (i < height - 1) {
                int pixelVal = raw_data[centerPixel + step];
                ySum += 2 * pixelVal;
            }
            if (i < height - 1 && j < width - 1) {
                int pixelVal = raw_data[centerPixel + step + 1];
                xSum += pixelVal;
                ySum += pixelVal;
            }
            xGrad[i * width + j] = xSum;
            yGrad[i * width + j] = ySum;
        }
    }
}

void getHarrisResponse(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int windowSize) {
    /* Assigns corner score based on Harris Corner Metric. This assumes that all corners are at least 1 from the edge. 
    Outside of ORB, this is NOT a safe assumption, but in ORB all corners must not be on the end because FAST does not
    consider these points */
    int width = img.cols; int height = img.rows;
    int prod = width * height;
    int *xGrad = (int*) malloc(sizeof(int) * prod);
    int *yGrad = (int*) malloc(sizeof(int) * prod);
    int radius = windowSize / 2;

    float scale = 1.f/((1 << 2) * windowSize * 255.f);
    float scale_sq_sq = scale * scale * scale * scale;

    applyKernel(xGrad, yGrad, img);
    for (auto& keypoint : keypoints) {
        long A = 0;
        long B = 0;
        long C = 0;
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                int idx, x, y;
                idx = (keypoint.pt.y + i) * width + (keypoint.pt.x + j);
                x = xGrad[idx];
                y = yGrad[idx];
                A += x * x;
                B += y * y;
                C += x * y;
            }
        }
        // return the Corner Response, which is Det - k Tr^2. Det is A * B - C^2, and Tr is A + B
        keypoint.response = (((A * B) - (C * C)) - (RESPONSE_CONSTANT * (A + B) * (A + B))) * scale_sq_sq;
    }
    free(xGrad);
    free(yGrad);
}


// int main() {
//     cv::Mat img = cv::imread("euroc.jpg", cv::IMREAD_GRAYSCALE);
//     int pixels = img.size().height * img.size().width;

//     int *xGrad = (int*) malloc(sizeof(int) * pixels);
//     int *yGrad = (int*) malloc(sizeof(int) * pixels);

//     const auto& start = std::chrono::high_resolution_clock::now();
//     applyKernel(xGrad, yGrad, img);
//     const auto& end = std::chrono::high_resolution_clock::now();
//     const auto& duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//     std::cout << "Filter took " << duration.count() << " microseconds to execute." << std::endl;

//     cv::Mat derivativeImage(img.size().height, img.size().width, CV_8UC1, cv::Scalar(0));
//     int width = img.size().width;
//     for (int i = 0; i < img.size().height; i++) {
//         for (int j = 0; j < width; j++) {
//             int value = std::round(sqrt(std::pow(xGrad[i * width + j], 2) + std::pow(yGrad[i * width + j], 2)));
//             derivativeImage.at<uchar>(i, j) = value < 0 ? 0 : (value > 255 ? 255 : value);
//         }
//     }

//     cv::imwrite("demo_result.png", derivativeImage);
//     // cv::waitKey(0);

//     free(xGrad);
//     free(yGrad);
// }