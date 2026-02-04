#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "src/fast.hpp"

using namespace std;

bool blur(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev) {
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
    return true;
}


int main() {
    cv::Mat read = cv::imread("train_images/weitz.png", cv::IMREAD_GRAYSCALE);
    cv::Mat src;
    std::vector<cv::Point2i> points;
    cv::resize(read, src, cv::Size(1920,1080));
    cv::Mat dest;
    auto start_time = std::chrono::system_clock::now();
    blur(src, dest, 2, 1);
    learnedFast(points, src, 10);
    auto end_time = std::chrono::system_clock::now();
    cout << "Homemade Gaussian Blur took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    cv::Mat cvDest;
    std::vector<cv::KeyPoint> cvPoints;
    start_time = std::chrono::system_clock::now();
    cv::GaussianBlur(src, cvDest, cv::Size(5, 5), 1);
    cv::FAST(src, cvPoints, 10, false);
    end_time = std::chrono::system_clock::now();
    cout << "OpenCV Gaussian Blur took " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds." << endl;

    cv::imshow("Blurred", dest);
    cv::imshow("CV", cvDest);
    cv::waitKey(0);

    for (int i = 4; i < src.size().height - 4; i++) {
        for (int j = 4; j < src.size().width - 4; j++) {
            if (abs(dest.at<uchar>(i, j) - cvDest.at<uchar>(i, j)) >= 2) {
                cout << (int) dest.at<uchar>(i, j) << " " << (int) cvDest.at<uchar>(i, j) << endl;
            }
        }
    }

    cout << (points.size() == cvPoints.size()) << endl;

    return 0;
}