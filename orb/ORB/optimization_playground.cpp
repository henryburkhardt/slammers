#include <opencv2/opencv.hpp>
#include "src/processing.hpp"

using namespace std;

struct CompareKeyPoint {
    bool operator()(const cv::KeyPoint& kp1, const cv::KeyPoint& kp2) {
        return kp1.response > kp2.response; // Min-heap based on response
    }
};


int main() {
    const auto& img = cv::imread("demo_img.png", cv::IMREAD_GRAYSCALE);
    std::vector<cv::KeyPoint> kptArray;
    kptArray.push_back(cv::KeyPoint(cv::Point2i(250, 250), 7));
    cv::Mat dest_img;
    cv::cvtColor(img, dest_img, cv::COLOR_GRAY2BGR);

    cout << kptArray.size() << endl;

    for (const auto& kpt : kptArray) {
        cv::circle(dest_img, kpt.pt, 0, cv::Scalar(0, 255, 0), -1);
    }

    calculateOrientation(img, kptArray[0]);

    cout << kptArray[0].angle << endl;
    cout << atan2(0, 0) << endl;
    // cv::imshow("result", dest_img);
    // cv::waitKey(0);
}
