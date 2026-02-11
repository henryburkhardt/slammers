#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>
#include <cmath>
#include <filesystem>
#include <vector>

/**
 * An array of points representing the points of the circle with radius 3 relative to a given center pixel.
 */
const int CIRCLE_POINTS[16][2] = {
    {-1, -3},
    {0, -3},
    {1, -3},
    {2, -2},
    {3, -1},
    {3, 0},
    {3, 1},
    {2, 2},
    {1, 3},
    {0, 3},
    {-1, 3},
    {-2, 2},
    {-3, 1},
    {-3, 0},
    {-3, -1},
    {-2, -2}
};

std::vector<uchar> getSurroundingPoints(const cv::Mat& img, int row, int col) {
    uchar* raw_data = img.data;
    
    int width = img.step;
    int center_point = row * width + col;

    std::vector<uchar> vals = {
        raw_data[center_point - 3 * width - 1],
        raw_data[center_point - 3 * width],
        raw_data[center_point - 3 * width + 1],
        raw_data[center_point - 2 * width + 2],
        raw_data[center_point - width + 3],
        raw_data[center_point + 3],
        raw_data[center_point + width + 3],
        raw_data[center_point + 2 * width + 2],
        raw_data[center_point + 3 * width + 1],
        raw_data[center_point + 3 * width],
        raw_data[center_point + 3 * width - 1],
        raw_data[center_point + 2 * width - 2],
        raw_data[center_point + width - 3],
        raw_data[center_point - 3],
        raw_data[center_point - width - 3],
        raw_data[center_point - 2 * width - 2],
        raw_data[center_point]
    };

    return vals;
}

int get_corner_score(const cv::Mat& img, int row, int col) {
    auto pointVector = getSurroundingPoints(img, row, col);

    for (auto point : pointVector) {
        std::cout << (int) point << " ";
    }
    std::cout << std::endl;

    uchar center = pointVector[16];
    int best = 0;
    for (int i = 0; i < 16; i++) {
        int max_diff = INT_MIN;
        int min_diff = INT_MAX;
        for (int j = 0; j < 9; j++) {
            int diff = pointVector[(i + j) % 16] - center;
            max_diff = std::min(0, std::max(max_diff, diff));
            min_diff = std::max(0, std::min(min_diff, diff));
        }
        best = std::max(best, -max_diff);
        best = std::max(best, min_diff);
    }
    return best - 1;
}

/**
 * A detector that utilizes the FAST algorithm without machine learning (so it's not really FAST). Checks whether
 * a pixel is surrounded by a contiguous sequence of n points that are either brighter or darker than the center pixel
 * by a value greater than threshold. When n is greater than or equal to 12, this code allows for the high-speed test 
 * outlined in Rosten's paper regarding FAST.
 */
class FASTDetector {
    public:
        int n;  // the number of consecutive points that should be above or below threshold
        int threshold;  // the delta for a corner to be considered substantially lighter or darker than its surroundings
        /**
         * Constructor for a FASTDetector object
         * @param n the number of consecutive pixels that are brighter or darker for a pixel to classify as a corner
         * @param threshold the amount a pixel must be brighter or darker by to be called bright or dark
         */
        FASTDetector(int n = 12, int threshold = 10) {
            this->n = n;
            this->threshold = threshold;
        }

        /**
         * Performs FAST for each pixel in an image. If n >= 12 it will perform a high-speed candidate test before fully
         * checking each pixel.
         * @param img an OpenCV Grayscale Image that FAST should be performed on.
         */
        std::vector<cv::Point> detect(cv::Mat img) {
            std::vector<cv::Point> keypoints;
            int width = img.size().width;
            int height = img.size().height;
            for (int i = 3; i < height - 3; i++) {
                for (int j = 3; j < width - 3; j++) {
                    if (n >= 12) {
                        // first check if the pixel could be a corner, then check if it is one.
                        if (checkIfCandidate(i, j, img) && isKeypoint(i, j, img))
                            keypoints.push_back(cv::Point(j, i));
                    } else if (isKeypoint(i, j, img)){
                        keypoints.push_back(cv::Point(j, i));
                    }
                }
            }
            return keypoints;
        }

    private:
    /**
     * Implementation of the high-speed test outlined in Rosten's paper. First checks opposite pixels. If they are
     * both within the threshold it can't be a corner. If that test passes, then it checks each of the points and if
     * three out of four are lighter or three out of four are darker it performs a full test. Otherwise, it could not be
     * a corner.
     * @param row the row of the pixel to check
     * @param col the column of the pixel to check
     * @param img an OpenCV Image loaded in grayscale.
     */
        bool checkIfCandidate(int row, int col, cv::Mat img) {
            int baseline = img.at<uchar>(row, col);
            int num_darker = 0;
            int num_lighter = 0;
            int topMiddle = img.at<uchar>(row, col - 3);
            int bottomMiddle = img.at<uchar>(row, col + 3);
            int rightMiddle = img.at<uchar>(row + 3, col);
            int leftMiddle = img.at<uchar>(row - 3, col);
            // check for opposite corners not both being within threshold
            if ((topMiddle - threshold < baseline && 
                topMiddle + threshold > baseline &&
                bottomMiddle - threshold < baseline &&
                bottomMiddle + threshold > baseline) ||
                (rightMiddle - threshold < baseline && 
                rightMiddle + threshold > baseline &&
                leftMiddle - threshold < baseline &&
                leftMiddle + threshold > baseline)) {
                    return false;
                }
            
            // count the number of lighter and darker pixels and return whether one of them is at least three.
            if (topMiddle - threshold > baseline) num_lighter++;
            else if (topMiddle + threshold < baseline) num_darker++;

            if (bottomMiddle - threshold > baseline) num_lighter++;
            else if (bottomMiddle + threshold < baseline) num_darker++;

            if (leftMiddle - threshold > baseline) num_lighter++;
            else if (leftMiddle + threshold < baseline) num_darker++;

            if (rightMiddle - threshold > baseline) num_lighter++;
            else if (rightMiddle + threshold < baseline) num_darker++;

            return num_darker >= 3 || num_lighter >= 3;
        }

        /**
         * Performs full test to see whether there are n consecutive surrounding pixels around a target pixel.
         * @param row row of target pixel
         * @param col column of target pixel
         * @param img OpenCV Grayscale Image to check pixels from.
         */
        bool isKeypoint(int row, int col, cv::Mat img) {
            int consecutiveDarker = 0;
            int consecutiveLighter = 0;
            int startDarker = 0;
            int startLighter = 0;

            int baseline = img.at<uchar>(row, col);

            for (int i = 0; i < 16; i++) {
                int color = img.at<uchar>(row + CIRCLE_POINTS[i][1], col + CIRCLE_POINTS[i][0]);
                // pixel is darker
                if(color + threshold < baseline) {
                    consecutiveLighter = 0;
                    consecutiveDarker ++;
                    if (i == startDarker) startDarker++;
                }
                // pixel is lighter
                else if(color - threshold > baseline) {
                    consecutiveDarker = 0;
                    consecutiveLighter++;
                    if (i == startLighter) startLighter++;
                }
                // pixel is similar to reference pixel
                else {
                    consecutiveDarker = 0;
                    consecutiveLighter = 0;
                }
                if (consecutiveDarker == n || consecutiveLighter == n) return true;
            }

            return (consecutiveDarker + startDarker >= n || consecutiveLighter + startLighter >= n);
        }
};

/**
 * Part of Machine Learning framework for FAST. The goal is to minimize entropy. That is, within a set of pixels, we
 * want them to all be corners or all be non-corners. This calculates the entropy of a set of pixels
 * @param set the set of pixels that are being used to calculate entropy
 * @param corners the classification of said pixels for entropy calculation
 */
double calculateEntropy(const std::vector<cv::Point3d>& set, std::vector<std::vector<std::vector<bool>>>& corners) {
    int numCorners = 0;
    int numNoncorners = 0;
    for (const auto& pixel : set) {
        if (corners[pixel.x][pixel.y][pixel.z]) numCorners++;
        else numNoncorners++;
    }

    if (numCorners == 0 || numNoncorners == 0) return 0;

    int setSize = set.size();
    return setSize * std::log2(setSize) - numCorners * std::log2(numCorners) - numNoncorners * std::log2(numNoncorners);
}

/**
 * Recursive function as part of the Machine Learning framework of FAST. This uses the ID3 algorithm to generate a
 * decision tree that can guide a program about which pixels must be examined to determine whether a pixel is a corner.
 * Importantly, this prints the resulting tree to standard output rather than returning it as an object.
 * @param trainImages a vector of OpenCV Grayscale Images that are used as a training reference
 * @param pixelSet the set of pixels to operate on. This starts as the entire set of pixels, but shrinks with recursion
 * @param imageCorners the classifications for whether a given pixel is a corner
 * @param validChoices the set of moves. This prevents computation on the same pixel twice
 * @param threshold the FAST training threshold for the dataset
 * @param depthTracker a tracker of recursion depth for outputting data in a tree format.
 */
void trainDecisionTree(std::vector<cv::Mat>& trainImages, std::vector<cv::Point3d>& pixelSet, std::vector<std::vector<std::vector<bool>>>& imageCorners, std::vector<int> validChoices, int threshold, int depthTracker) {
    // keep track of best values to prevent expensive recomputation.
    double bestInfoGain = -1;
    int currentDecision = -1;
    std::vector<cv::Point3d> bestSimilar;
    std::vector<cv::Point3d> bestDarker;
    std::vector<cv::Point3d> bestLighter;
    double bestSimEntropy = 0;
    double bestDarkEntropy = 0;
    double bestLightEntropy = 0;
    
    // for each choice that does not repeat a previously chosen entry
    for (int choice : validChoices) {
        std::vector<cv::Point3d> similarPixels;
        std::vector<cv::Point3d> darkerPixels;
        std::vector<cv::Point3d> lighterPixels;
        // partition pixels into darker, lighter, or similar based on the current pixel choice.
        for (const auto& pixel : pixelSet) {
            const auto& img = trainImages[pixel.x];
            int y = pixel.y;
            int z = pixel.z;
            if (img.at<uchar>(y + CIRCLE_POINTS[choice][1], z + CIRCLE_POINTS[choice][0]) + threshold < img.at<uchar>(y, z)) darkerPixels.push_back(pixel);
            else if (img.at<uchar>(y + CIRCLE_POINTS[choice][1], z + CIRCLE_POINTS[choice][0]) - threshold > img.at<uchar>(y, z)) lighterPixels.push_back(pixel);
            else similarPixels.push_back(pixel);
        }
        
        // calculate entropy and the amount of information we gain by choosing this one.
        double allEntropy = calculateEntropy(pixelSet, imageCorners);
        double simEntropy = calculateEntropy(similarPixels, imageCorners);
        double darkEntropy = calculateEntropy(darkerPixels, imageCorners);
        double lightEntropy = calculateEntropy(lighterPixels, imageCorners);
        double infoGain = allEntropy - darkEntropy - simEntropy - lightEntropy;

        // if we gain more information than the current best choice, we update the best choice.
        if (infoGain > bestInfoGain) {
            currentDecision = choice;
            bestInfoGain = infoGain;
            bestSimilar = similarPixels;
            bestDarker = darkerPixels;
            bestLighter = lighterPixels;
            bestSimEntropy = simEntropy;
            bestDarkEntropy = darkEntropy;
            bestLightEntropy = lightEntropy;
        }
    }

    // print the choice we have made to standard output and remove it from the valid choices array
    for (int i = 0; i < depthTracker; i++) std::cout << " ";
    std::cout << currentDecision << std::endl;
    validChoices.erase(std::remove(validChoices.begin(), validChoices.end(), currentDecision), validChoices.end());

    // recur on just the pixels that were classified as similar in the previous example
    if (bestSimEntropy != 0) trainDecisionTree(trainImages, bestSimilar, imageCorners, validChoices, threshold, depthTracker + 1);
    else {
        for (int i = 0; i < depthTracker + 1; i++) std::cout << " ";
        if (bestSimilar.size() > 0 && imageCorners[bestSimilar[0].x][bestSimilar[0].y][bestSimilar[0].z]) std::cout << "corner" << std::endl;
        else std::cout << "not corner" << std::endl;
    }

    // recur on just the pixels that were classified as darker in the previous example
    if (bestDarkEntropy != 0) trainDecisionTree(trainImages, bestDarker, imageCorners, validChoices, threshold, depthTracker + 1);
    else {
        for (int i = 0; i < depthTracker + 1; i++) std::cout << " ";
        if (bestDarker.size() > 0 && imageCorners[bestDarker[0].x][bestDarker[0].y][bestDarker[0].z]) std::cout << "corner" << std::endl;
        else std::cout << "not corner" << std::endl;
    }

    // recur on just the pixels that were classified as lighter in the previous example
    if (bestLightEntropy != 0) trainDecisionTree(trainImages, bestLighter, imageCorners, validChoices, threshold, depthTracker + 1);
    else {
        for (int i = 0; i < depthTracker + 1; i++) std::cout << " ";
        if (bestLighter.size() > 0 && imageCorners[bestLighter[0].x][bestLighter[0].y][bestLighter[0].z]) std::cout << "corner" << std::endl;
        else std::cout << "not corner" << std::endl;
    }
}

/**
 * Train a Decision Tree on the images in the train_images subdirectory.
 */
void train(int n, int threshold) {
    // load images into images vector and calculate keypoints using the slow detector.
    const auto itr = std::filesystem::directory_iterator("train_images");
    std::vector<cv::Mat> images;
    std::vector<std::vector<std::vector<bool>>> keypoints;
    for (const auto& file : itr) {
        cv::Mat img = cv::imread(file.path(), cv::IMREAD_GRAYSCALE);
        images.push_back(img);
        FASTDetector detector = FASTDetector(n, threshold);
        std::vector<cv::Point> imgKeypoints = detector.detect(img);

        std::vector<std::vector<bool>> keypointBoolArray = std::vector<std::vector<bool>>(img.size().height, std::vector<bool>(img.size().width, false));
        for (cv::Point point : imgKeypoints) {
            keypointBoolArray[point.y][point.x] = true;
        }
        keypoints.push_back(keypointBoolArray);
    }

    // generate initial possible choices (can choose any point on circle at beginning)
    std::vector<int> choices = std::vector<int>(16, 0);
    for (int i = 0; i < 16; i++) {
        choices[i] = i;
    }

    // generate initial set of pixels, which starts as all pixels the slow implementation looks at.
    std::vector<cv::Point3d> pixelSet;
    for (size_t i = 0; i < images.size(); i++) {
        const auto& img = images[i];
        int height = img.size().height;
        int width = img.size().width;
        for (int j = 3; j < height - 3; j++) {
            for (int k = 3; k < width - 3; k++) {
                pixelSet.push_back(cv::Point3d(i, j, k));
            }
        }
    }

    trainDecisionTree(images, pixelSet, keypoints, choices, threshold, 0);
}

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "train") == 0) {
        train(9, 10);
        return 0;
    }
    cv::Mat img = cv::imread("train_images/libe.png", cv::IMREAD_GRAYSCALE);

    // Check if the image loaded successfully
    if (img.empty()) {
        std::cerr << "Error: Could not open or find the image." << std::endl;
        return -1;
    }

    FASTDetector detector = FASTDetector(9, 10);
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<cv::Point> keypoints= detector.detect(img);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "nonML took " << duration.count() << " microseconds to execute." << std::endl;
    std::cout << keypoints.size() << std::endl;

    std::vector<cv::KeyPoint> kps;
    auto thing = cv::FastFeatureDetector::create(43, true);
    
    start = std::chrono::high_resolution_clock::now();
    thing->detect(img, kps);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "OpenCV took " << duration.count() << " microseconds to execute." << std::endl;
    std::cout << kps.size() << std::endl;
    std::cout << kps[0].response << std::endl;
    std::cout << get_corner_score(img, kps[0].pt.y, kps[0].pt.x) << std::endl;

    cv::Mat colorImg;
    cv::cvtColor(img, colorImg, cv::COLOR_GRAY2BGR);

    for (auto keypoint : kps) {
        colorImg.at<cv::Vec3b>(keypoint.pt) = cv::Vec3b(0, 0, 255);
    }

    cv::imshow("Frame", colorImg);
    cv::waitKey(0);

    return 0;
}