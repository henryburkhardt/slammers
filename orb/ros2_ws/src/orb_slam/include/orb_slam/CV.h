#include <opencv2/opencv.hpp>

void blur(const cv::Mat& src, cv::Mat& dest, int kernelRadius, double stdDev);

void learnedFast(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int threshold, bool nonMaxSuppression = true);