#include "fast.hpp"
#include "harris.hpp"
#include <opencv2/opencv.hpp>
#include "processing.hpp"
#include "briefpattern.hpp"
#include "orb.hpp"
#include <bit>

KeyPoint::KeyPoint(cv::Point2i point, double score) {
    this->point = point;
    this->score = score;
}

bool KeyPoint::operator<(const KeyPoint& other) const {
    return score < other.score;
}


void OrbPoint::compute_descriptor(const cv::Mat& img) {
    int index;
    int stepSize = img.step;
    uchar* raw_data = img.data;

    if (orientation < 0) orientation += 2 * M_PI;
    index = cvRound(orientation / (M_PI / 15)) % 30;
    for(int i = 0; i < 4; i++) {
        long long descriptor = 0;
        for(int j = 0; j < 64; j++) {
            int x_1 = point.x + PATTERN[index][i * 64 + j][0];
            int y_1 = point.y + PATTERN[index][i * 64 + j][1];
            int x_2 = point.x + PATTERN[index][i * 64 + j][2];
            int y_2 = point.y + PATTERN[index][i * 64 + j][3];
            descriptor |= (raw_data[y_1 * stepSize + x_1] >= raw_data[y_2 * stepSize + x_2]) << j;
        }
        descriptors[i] = descriptor;
    }
}

std::vector<cv::Point2i> selectKeypoints(const cv::Mat& img) {
    int width = img.size().width;
    int height = img.size().height;

    std::vector<cv::Point2i> keypoints;
    learnedFast(keypoints, img, 10);

    if (keypoints.size() > NUMBER_KEYPOINTS) {
        int *xGrad = (int *) malloc(sizeof(int) * width * height);
        int *yGrad = (int *) malloc(sizeof(int) * width * height);
        applyKernel(xGrad, yGrad, img);

        std::priority_queue<KeyPoint> pq;

        for (int i = 0; i < keypoints.size(); i++) {
            pq.push(KeyPoint(keypoints[i], getCornerScore(xGrad, yGrad, keypoints[i].y, keypoints[i].x, width)));
        }

        for (int i = 0; i < NUMBER_KEYPOINTS; i++) {
            keypoints[i] = pq.top().point;
            pq.pop();
        }
        keypoints.erase(keypoints.begin() + NUMBER_KEYPOINTS, keypoints.end());
    }
    
    return keypoints;
}

int hammingDistance(const OrbPoint& point1, const OrbPoint& point2) {
    int distance = 0;
    for (int i = 0; i < 4; i++) {
        long long difference = point1.descriptors[i] ^ point2.descriptors[i];
        distance += __builtin_popcount(difference);
    }
    return distance;
}

std::vector<OrbPoint> processOrbImage(const cv::Mat& img) {
    cv::Mat blurredImage;
    blurImage(img, blurredImage, 2, 2);
    std::vector<cv::Point2i> keypoints = selectKeypoints(img);

    std::vector<OrbPoint> orbpoints = std::vector<OrbPoint>(keypoints.size(), OrbPoint());

    for (int k = 0; k < keypoints.size(); k++) {
        orbpoints[k].point = keypoints[k];
        orbpoints[k].orientation = calculateOrientation(img, keypoints[k]);
        orbpoints[k].compute_descriptor(img);
    }

    return orbpoints;
}

// int main() {
//     cv::Mat img = cv::imread("src/train_images/libe.png", cv::IMREAD_GRAYSCALE);
    
//     // processOrbImage(img);
//     cv::imshow("Not Blurred", img);
//     cv::Mat blurred = blurImage(img);
//     cv::imshow("Blurred", blurred);
//     cv::waitKey(0);
// }