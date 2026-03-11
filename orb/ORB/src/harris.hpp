#ifndef HARRIS_CLASS
#define HARRIS_CLASS

#include <opencv2/opencv.hpp>

void getHarrisResponse(const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, int windowSize);
void applyKernel(int *xGrad, int *yGrad, const cv::Mat& img);

#endif