#ifndef FAST_HEADER
#define FAST_HEADER

#include <opencv2/opencv.hpp>

void learnedFast(std::vector<cv::Point2i>& keypoints, const cv::Mat& img, int threshold);

#endif