#include <opencv2/opencv.hpp>

const int CENTROID_RADIUS = 3;

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
    return true;
}

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