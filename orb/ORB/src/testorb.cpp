#include "orb.hpp"
#include <random>
#include <opencv2/opencv.hpp>

OrbPoint findBRIEFmatch(const OrbPoint& base, const std::vector<OrbPoint>& potentialMatches) {
    int minDistance = 257;
    OrbPoint bestMatch;
    for (const auto& match : potentialMatches) {
        int distance = hammingDistance(base, match);
        std::cout << distance << std::endl;
        if (distance < minDistance) {
            minDistance = distance;
            bestMatch = match;
        }
    }
    // std::cout << minDistance << std::endl;
    return bestMatch;
}

int main() {
    cv::Mat im1 = cv::imread("im1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat im2 = cv::imread("im2.png", cv::IMREAD_GRAYSCALE);

    // cv::Mat colorim1;
    // cv::Mat colorim2;

    // cv::cvtColor(im1, colorim1, cv::COLOR_GRAY2BGR);
    // cv::cvtColor(im2, colorim2, cv::COLOR_GRAY2BGR);

    cv::Mat endDisplay;
    cv::Mat colorEndDisplay;

    std::vector<OrbPoint> im1_points = processOrbImage(im1);
    std::vector<OrbPoint> im2_points = processOrbImage(im2);

    // for (const auto& point : im2_points) {
    //     std::out
    //     colorim2.at<cv::Vec3i>(point.point) = cv::Vec3i(0, 0, 255);
    // }

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, im1_points.size() - 1);

    OrbPoint compPoint = im1_points[dist(gen)];
    OrbPoint match = findBRIEFmatch(compPoint, im2_points);
    match.point.x += im1.size().width;


    cv::hconcat(im1, im2, endDisplay);
    cv::cvtColor(endDisplay, colorEndDisplay, cv::COLOR_GRAY2RGB);
    cv::line(colorEndDisplay, compPoint.point, match.point, cv::Scalar(0, 0, 255), 5);
    cv::imshow("concatenated images", colorEndDisplay);
    cv::waitKey(0);

    // unsigned long long x = 0xFFFFFFFFFFFFFFFF;
    // unsigned long long y = 0xFFFFFFFFFFFFFFFE;
    // std::cout << __builtin_popcount(x ^ y) << std::endl;
}