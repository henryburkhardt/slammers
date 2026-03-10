#include <opencv2/opencv.hpp>
#include "processing.hpp"

void boxFilter(cv::Mat& img, cv::Mat& dest) {
    dest.create(img.size(), CV_8U);

    int width = img.size().width; int height = img.size().height;
    int pixels = (width + 1) * (height + 1);
    uchar *raw_data = img.data;
    uchar *dest_data = dest.data;

    long long* integralImage = (long long *) malloc(sizeof(long long) * pixels);
    #define INDEX(row, col) ((row) * (width + 1) + col)

    for (int i = 0; i < width; i++) {
        integralImage[i] = 0;
    }

    for (int i = 0; i < height; i++) {
        long long rowSum = 0;
        integralImage[INDEX(i, 0)] = 0;
        for (int j = 0; j < width; j++) {
            rowSum += raw_data[i * width + j];
            integralImage[INDEX(i + 1, j + 1)] = rowSum + integralImage[INDEX(i, j + 1)];
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int ty = std::max(i - 2, 0);
            int by = std::min(i + 3, height);
            int lx = std::max(j - 2, 0);
            int rx = std::min(j + 3, width);

            long long sum = integralImage[INDEX(by, rx)] + integralImage[INDEX(ty, lx)] - integralImage[INDEX(ty, rx)] - integralImage[INDEX(by, lx)];
            int count = (rx - lx) * (by - ty);
            dest_data[i * width + j] =  (sum + count / 2) / count;
        }
    }

    #undef INDEX

    free(integralImage);
}

void gaussianBlur(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev) {
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
    cv::Mat temp(height, width, CV_64F, cv::Scalar(0.0));

    uchar *src_data = src.data;
    uchar *temp_data = temp.data;
    uchar *dest_data = dest.data;

    int src_step = src.step;
    int temp_step = temp.step;
    int dest_step = dest.step;

    // perform horizontal filter application over image to temp.
    for (int i = 0; i < height; i++) {
        double *temp_pointer = reinterpret_cast<double*>(temp_data + i * temp_step); // because this is double instead of uchar, it is handled specially.
        for (int j = 0; j < width; j++) {
            double pixelValue = 0;
            int startIdx = j >= kernelRadius ? -kernelRadius : -j;
            int endIdx = j < width - kernelRadius ? kernelRadius : width - j - 1;
            for (int k = startIdx; k <= endIdx; k++) {
                pixelValue += src_data[i * src_step + j + k] * kernel[k + kernelRadius];
            }
            // set temp image to computed value and increment index for next pixel
            temp_pointer[j] = pixelValue;
        }
    }

    // perform vertical pass kernel application from temp to dest.
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double pixelValue = 0;
            int startIdx = i >= kernelRadius ? -kernelRadius : -i;
            int endIdx = i < height - kernelRadius ? kernelRadius : height - i - 1;
            for (int k = startIdx; k <= endIdx; k++) {
                double *temp_pointer = reinterpret_cast<double*>(temp_data + (i + k) * temp_step);
                pixelValue += temp_pointer[j] * kernel[k + kernelRadius];
            }
            // set temp image to computed value and increment index for next pixel
            dest_data[i * dest_step + j] = cvRound(pixelValue);
        }
    }
}


int main() {
    cv::Mat img = cv::imread("train_images/weitz.png", CV_8U);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    cv::Mat cvFilter;
    cv::boxFilter(img, cvFilter, -1, cv::Size(5, 5));

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "CV Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    

    begin = std::chrono::steady_clock::now();

    cv::Mat myFilter;
    boxFilter(img, myFilter);

    end = std::chrono::steady_clock::now();

    std::cout << "My Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    begin = std::chrono::steady_clock::now();

    cv::Mat cvGaussian;
    cv::GaussianBlur(img, cvGaussian, cv::Size(5, 5), 2);

    end = std::chrono::steady_clock::now();
    std::cout << "OpenCV Gaussian Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;


    begin = std::chrono::steady_clock::now();

    cv::Mat myGaussian;
    gaussianBlur(img, myGaussian, 2, 2);

    end = std::chrono::steady_clock::now();
    std::cout << "Gaussian Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    cv::imshow("Filtered", myGaussian);
    // cv::imshow("CvVersion", cvFilter);
    cv::waitKey(0);

    for (int i = 2; i < img.size().height - 2; i++) {
        for (int j = 2; j < img.size().width - 2; j++) {
            if (myGaussian.at<uchar>(i, j) != cvGaussian.at<uchar>(i, j)) {
                std::cout << (int) myGaussian.at<uchar>(i, j) << " " << (int) cvGaussian.at<uchar>(i, j) << " " << i << " " << j << std::endl;
            }
        }
    }
}