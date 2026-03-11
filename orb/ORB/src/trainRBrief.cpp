#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "processing.hpp"
#include "harris.hpp"
// #include "orb.cpp"

// how many FAST keypoints from dataset to analyze
size_t SAMPLE_SIZE = 300000;
// how many images to check for FAST keypoints. Set high to analyze whole dataset.
size_t IMAGE_LIMIT = 99999999999999;
size_t BRIEF_TESTS = 256;
double CORRELATION_INCREMENT = 0.05;


struct imagePointPair {
    int pictureIndex;
    cv::KeyPoint keypoint;
    int rotationIndex;
    imagePointPair(int index, cv::KeyPoint point) {
        pictureIndex = index;
        keypoint = point;
    }
};

class PointTest {
    public:
        // these are the points for each rotation of the test, each index represents a rotation of the point by 12 degrees. 
        std::vector<cv::Point2i> point1, point2;
        int score = 0;
        double highestCorrelation = -2.0;
        size_t correlationComparisons = 0;

        PointTest(cv::Point2i p1, cv::Point2i p2) {
            point1.push_back(p1);
            point2.push_back(p2);
            double rotationMatrix[2][2];
            for (double angle = M_PI / 15; angle < 2 * M_PI; angle += M_PI / 15) {
                rotationMatrix[0][0] = cos(angle);
                rotationMatrix[0][1] = -sin(angle);
                rotationMatrix[1][0] = sin(angle);
                rotationMatrix[1][1] = cos(angle);
                int rotated_x = round(p1.x * rotationMatrix[0][0] + p1.y * rotationMatrix[0][1]);
                int rotated_y = round(p1.x * rotationMatrix[1][0] + p1.y * rotationMatrix[1][1]);
                point1.push_back(cv::Point2i(rotated_x, rotated_y));
                rotated_x = round(p2.x * rotationMatrix[0][0] + p2.y * rotationMatrix[0][1]);
                rotated_y = round(p2.x * rotationMatrix[1][0] + p2.y * rotationMatrix[1][1]);
                point2.push_back(cv::Point2i(rotated_x, rotated_y));
            }
        }

        void runTest(const cv::Mat& img, cv::Point2i point, int idx) {
            int width = img.size().width; int height = img.size().height;
            int row = point.y; int col = point.x;
            size_t stepSize = img.step;

            int x_1 = col + point1[idx].x;
            int y_1 = row + point1[idx].y;
            int x_2 = col + point2[idx].x;
            int y_2 = row + point2[idx].y;
            if (x_1 < 0 || x_1 >= width || x_2 < 0 || x_2 >= width || y_1 < 0 || y_1 >= height || y_2 < 0 || y_2 >= height) return;
            unsigned char* raw_data = img.data;
            int comp = raw_data[y_1 * stepSize + x_1] - raw_data[y_2 * stepSize + x_2];
            if (comp >= 0) {
                score += 1;
            }
            else {
                score -= 1;
            }
        }

        void calculateResults(std::vector<cv::Mat>& images, std::vector<imagePointPair>& points, std::vector<int>& results) {
            for (const auto& point : points) {
                const cv::Mat& targetPicture = images[point.pictureIndex];
                int width = targetPicture.size().width; int height = targetPicture.size().height;
                int stepSize = targetPicture.step;

                int idx = point.rotationIndex;
                const cv::KeyPoint& keypoint = point.keypoint;

                int x1 = keypoint.pt.x + point1[idx].x;
                int y1 = keypoint.pt.y + point1[idx].y;
                int x2 = keypoint.pt.x + point2[idx].x;
                int y2 = keypoint.pt.y + point2[idx].y;
                if (x1 < 0 || x1 >= width || x2 < 0 || x2 >= width || y1 < 0 || y1 >= height || y2 < 0 || y2 >= height) {
                    results.push_back(0);
                    continue;
                }
                int comp = targetPicture.data[y1 * stepSize + x1] - targetPicture.data[y2 * stepSize + x2];
                if (comp >= 0) results.push_back(1);
                else results.push_back(-1);
            }
        }

        bool operator<(const PointTest& other) const {
            return abs(score) > abs(other.score);
        }
};

double correlation(std::vector<int> resultsA, std::vector<int> resultsB) {
    long tt = 0, tf = 0, ft = 0, ff = 0;
    for (size_t i = 0; i < resultsA.size(); i++) {
        if (resultsA[i] == 0 || resultsB[i] == 0) continue;
        if (resultsA[i] == 1 && resultsB[i] == 1) tt++;
        if (resultsA[i] == 1 && resultsB[i] == -1) tf++;
        if (resultsA[i] == -1 && resultsB[i] == 1) ft++;
        if (resultsA[i] == -1 && resultsB[i] == -1) ff++;
    }
    long numerator = double(tt * ff) - (tf * ft);
    double denominator = sqrt(double(tt + tf) * double(tt + ft) * double(tf + ff) * double(ft + ff));
    if (denominator == 0) return std::nan("");
    return numerator / denominator;
}

int main() {
    std::vector<PointTest> tests;
    // for each point center in 26 x 26 (676) box
    for (int i = 0; i < 676; i++) {
        for (int j = i + 1; j < 676; j++) {
            int i_x = i % 26 - 13;
            int i_y = i / 26 - 13;
            int j_x = j % 26 - 13;
            int j_y = j / 26 - 13;
            if (abs(i_x - j_x) >= 5 || abs(i_y - j_y) >= 5) {
                tests.push_back(PointTest(cv::Point2i(i_x, i_y), cv::Point2i(j_x, j_y)));
            }
        }
    }

    const auto itr = std::filesystem::directory_iterator("rBRIEFImages");
    std::vector<cv::Mat> images;
    std::vector<imagePointPair> keypoints;


    std::cerr << "Processing Images" << std::endl;
    size_t counter = 0;
    for (const auto& file : itr) {
        if (counter++ > IMAGE_LIMIT) break;
        cv::Mat img = cv::imread(file.path(), cv::IMREAD_GRAYSCALE);
        std::vector<cv::KeyPoint> imgKeypoints;
        learnedFast(img, imgKeypoints, 10, true);

        getHarrisResponse(img, imgKeypoints, 7);
        cv::KeyPointsFilter::retainBest(imgKeypoints, 500);

        cv::Mat dest;
        blurImage(img, dest, 2, 2);
        images.push_back(dest);

        for (const auto& keypoint : imgKeypoints) {
            keypoints.push_back(imagePointPair(images.size() - 1, keypoint));
        }
    }


    std::random_device rd;
    std::mt19937 gen(rd());

    for (size_t i = 0; i < keypoints.size(); i++) {
        std::uniform_int_distribution<int> dist(i, keypoints.size() - 1);
        int idx = dist(gen);
        imagePointPair temp = keypoints[idx];
        keypoints[idx] = keypoints[i];
        keypoints[i] = temp;
    }
    if (keypoints.size() > SAMPLE_SIZE) {
        keypoints.erase(keypoints.begin() + SAMPLE_SIZE, keypoints.end());
        keypoints.shrink_to_fit();
    }

    std::cerr << keypoints.size() << std::endl;

    std::cerr << "Running Tests" << std::endl;

    int pointCounter = 0;
    for (auto& pair : keypoints) {
        calculateOrientation(images[pair.pictureIndex], pair.keypoint);
        double orientation = pair.keypoint.angle;
        int idx = cvRound(orientation / 12) % 30;
        pair.rotationIndex = idx;
        for (auto& test : tests) {
            test.runTest(images[pair.pictureIndex], pair.keypoint.pt, pair.rotationIndex);
        }
        if (++pointCounter % 1000 == 0) std::cerr << "Tested " << pointCounter << " points." << std::endl;
    }

    std::cerr << "Picking Tests" << std::endl;

    std::priority_queue<PointTest> pq, losers;
    for (const auto& test : tests) {
        pq.push(test);
    }
    
    std::vector<PointTest> selectedTests;
    std::vector<std::vector<int>> savedResults;
    selectedTests.push_back(pq.top());
    std::vector<int> results;
    selectedTests[0].calculateResults(images, keypoints, results);
    savedResults.push_back(results);
    pq.pop();

    double differenceThreshold = CORRELATION_INCREMENT;
    while (selectedTests.size() < BRIEF_TESTS) {
        if (pq.size() == 0) {
            pq.swap(losers);
            std::priority_queue<PointTest>().swap(losers);
            differenceThreshold += CORRELATION_INCREMENT;
            std::cerr << "Difference threshold increased to " << differenceThreshold << "." << std::endl;
        }
        PointTest nextTest = pq.top();

        std::vector<int> newResults;
        if (nextTest.correlationComparisons < selectedTests.size()) {
            nextTest.calculateResults(images, keypoints, newResults);
        }
        
        for (size_t i = nextTest.correlationComparisons; i < selectedTests.size(); i++) {
            double corr = correlation(savedResults[i], newResults);
            nextTest.highestCorrelation = std::max(abs(corr), nextTest.highestCorrelation);
            nextTest.correlationComparisons++;
        }

        if (nextTest.highestCorrelation < differenceThreshold) {
            selectedTests.push_back(nextTest);
            savedResults.push_back(newResults);
            std::cerr << "Chose " << selectedTests.size() << " points." << std::endl;
        }
        else losers.push(nextTest);
        pq.pop();
    }

    for (const auto& test : selectedTests) {
        std::cout << test.point1[0].x << ", " << test.point1[0].y << ", " << test.point2[0].x << ", " << test.point2[0].y << std::endl;
    }
}