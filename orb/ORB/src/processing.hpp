#include <opencv2/opencv.hpp>

#ifndef PROCESSING_LIBRARY
#define PROCESSING_LIBRARY

void blurImage(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev);
double calculateOrientation(const cv::Mat& img, cv::Point2i point);

#endif