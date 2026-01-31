#include <opencv2/opencv.hpp>

#ifndef ORB_LIBRARY
#define ORB_LIBRARY

const int NUMBER_KEYPOINTS = 500;

class KeyPoint {
    public:
        cv::Point2i point;
        double score;
        KeyPoint(cv::Point2i point, double score);

        bool operator<(const KeyPoint& other) const;
};

class OrbPoint {
    public:
        cv::Point2i point;
        float orientation;
        unsigned long long descriptors[4];
        
        void compute_descriptor(const cv::Mat& img);
};

std::vector<cv::Point2i> selectKeypoints(const cv::Mat& img);

int hammingDistance(const OrbPoint& point1, const OrbPoint& point2);

std::vector<OrbPoint> processOrbImage(const cv::Mat& img);

#endif