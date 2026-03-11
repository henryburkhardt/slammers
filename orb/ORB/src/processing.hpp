#include <opencv2/opencv.hpp>

#ifndef PROCESSING_LIBRARY
#define PROCESSING_LIBRARY

void blurImage(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev);
void calculateOrientation(const cv::Mat& img, cv::KeyPoint& point);
void learnedFast(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int threshold, bool nonMaxSuppression = true);
#endif