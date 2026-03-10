#ifndef HARRIS_CLASS
#define HARRIS_CLASS

#include <opencv2/opencv.hpp>

double getCornerScore(int *xGrad, int *yGrad, int row, int col, int width);
void applyKernel(int *xGrad, int *yGrad, const cv::Mat& img);

#endif