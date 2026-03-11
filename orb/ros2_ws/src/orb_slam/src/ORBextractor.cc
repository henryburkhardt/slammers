/**
* This file is part of ORB-SLAM.
* It is based on the file orb.cpp from the OpenCV library (see BSD license below).
*
* Copyright (C) 2014 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <http://webdiis.unizar.es/~raulmur/orbslam/>
*
* ORB-SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*********************************************************************/


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

#include "ORBextractor.h"
#include "CV.h"

#include <rclcpp/rclcpp.hpp>


using namespace cv;
using namespace std;

namespace ORB_SLAM
{

const float HARRIS_K = 0.04f;

const int PATCH_SIZE = 31;
const int CENTROID_RADIUS = 15;
const int HALF_PATCH_SIZE = 15;
const int EDGE_THRESHOLD = 16;

static void
HarrisResponses(const Mat& img, vector<KeyPoint>& pts, int blockSize, float harris_k)
{
    int width = img.cols; int height = img.rows;
    int prod = width * height;
    int radius = blockSize / 2;

    uchar* raw_data = img.data;
    int step = img.step;

    float scale = 1.f/((1 << 2) * blockSize * 255.f);
    float scale_sq_sq = scale * scale * scale * scale;

    for (auto& keypoint : pts) {
        long A = 0;
        long B = 0;
        long C = 0;
        int x = keypoint.pt.x;
        int y = keypoint.pt.y;
        uchar* prev_pointer = &raw_data[(y - radius - 1) * step + x];
        uchar* curr_pointer = &raw_data[(y - radius) * step + x];
        uchar* next_pointer = &raw_data[(y - radius + 1) * step + x];
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                int idx, x, y;
                idx = (keypoint.pt.y + i) * width + (keypoint.pt.x + j);
                x = prev_pointer[1] + next_pointer[1] - prev_pointer[-1] - next_pointer[-1] + 2 * (curr_pointer[1] - curr_pointer[-1]);
                y = next_pointer[-1] + next_pointer[1] - prev_pointer[-1] - prev_pointer[1] + 2 * (next_pointer[0] - prev_pointer[0]);
                A += x * x;
                B += y * y;
                C += x * y;
            }
            prev_pointer = curr_pointer;
            curr_pointer = next_pointer;
            next_pointer = &raw_data[y + i + 2];
        }
        // return the Corner Response, which is Det - k Tr^2. Det is A * B - C^2, and Tr is A + B
        keypoint.response = (((A * B) - (C * C)) - (harris_k * (A + B) * (A + B))) * scale_sq_sq;
    }
}



static float IC_Angle(const Mat& image, Point2f pt,  const vector<int> & u_max)
{
    int step = image.step;
    unsigned char* raw_data = image.data;
    int row = pt.y;
    int col = pt.x;
    unsigned char* center = &raw_data[row * step + col];
    int moment10 = 0;
    int moment01 = 0;
    for (int i = 0; i <= CENTROID_RADIUS; i++) {
        for (int j = -u_max[i]; j <= u_max[i]; j++) {
            uchar top_intensity = center[-step * i + j];
            uchar bottom_intensity = center[step * i + j];
            moment10 += j * (bottom_intensity + top_intensity);
            moment01 += i * (bottom_intensity - top_intensity);
        }
    }

    return fastAtan2((float)moment01, (float)moment10);
}


const float factorPI = (float)(CV_PI/180.f);
static void computeOrbDescriptor(const KeyPoint& kpt,
                                 const Mat& img, const Point* pattern,
                                 uchar* desc)
{
    float angle = (float)kpt.angle*factorPI;
    float a = (float)cos(angle), b = (float)sin(angle);

    const uchar* center = &img.at<uchar>(cvRound(kpt.pt.y), cvRound(kpt.pt.x));
    const int step = (int)img.step;

    #define GET_VALUE(idx) \
        center[cvRound(pattern[idx].x*b + pattern[idx].y*a)*step + \
               cvRound(pattern[idx].x*a - pattern[idx].y*b)]


    for (int i = 0; i < 32; ++i, pattern += 16)
    {
        int t0, t1, val;
        t0 = GET_VALUE(0); t1 = GET_VALUE(1);
        val = t0 < t1;
        t0 = GET_VALUE(2); t1 = GET_VALUE(3);
        val |= (t0 < t1) << 1;
        t0 = GET_VALUE(4); t1 = GET_VALUE(5);
        val |= (t0 < t1) << 2;
        t0 = GET_VALUE(6); t1 = GET_VALUE(7);
        val |= (t0 < t1) << 3;
        t0 = GET_VALUE(8); t1 = GET_VALUE(9);
        val |= (t0 < t1) << 4;
        t0 = GET_VALUE(10); t1 = GET_VALUE(11);
        val |= (t0 < t1) << 5;
        t0 = GET_VALUE(12); t1 = GET_VALUE(13);
        val |= (t0 < t1) << 6;
        t0 = GET_VALUE(14); t1 = GET_VALUE(15);
        val |= (t0 < t1) << 7;

        desc[i] = (uchar)val;
    }

    #undef GET_VALUE
}


static int bit_pattern_31_[256*4] =
{
    7, -11, 4, 1,
    0, -5, 0, 4,
    -13, -9, -8, -6,
    -6, -8, -5, -2,
    1, -13, 1, 11,
    7, 10, 12, 12,
    -11, 4, -12, 9,
    12, 2, 10, 8,
    11, -7, -3, 9,
    5, -6, -12, -3,
    -13, 5, 11, 5,
    12, -13, 9, -8,
    -5, 7, -4, 12,
    -13, -12, -13, -7,
    7, 7, 7, 12,
    1, 0, 6, 1,
    3, -11, -6, 10,
    10, -4, 11, 1,
    -8, -1, -8, 5,
    4, -2, 5, 6,
    -3, -9, -3, 9,
    -11, -4, -10, 1,
    -13, 3, -8, 3,
    -11, -8, -13, -2,
    -7, -13, -2, -11,
    -5, -13, -8, -8,
    -9, 5, -4, 5,
    -2, -2, -7, 0,
    -3, 3, 2, 5,
    -4, -5, 1, 0,
    8, -3, -9, 12,
    12, -2, -8, 3,
    8, 0, 12, 5,
    4, -13, 3, -8,
    0, -3, 1, 12,
    -6, 3, -6, 8,
    12, -8, 12, -3,
    -13, 11, -8, 11,
    3, -9, 5, 11,
    -13, 7, -11, 12,
    7, -4, 12, -4,
    7, -13, 12, -11,
    3, 6, 9, 10,
    3, -10, 5, -5,
    9, 4, 4, 9,
    0, -11, 5, -9,
    2, -7, -2, -2,
    7, 8, -1, 12,
    -13, -12, 12, -12,
    -5, -13, 5, 4,
    2, -5, -8, 7,
    -8, -2, 7, 12,
    -11, -7, 8, 5,
    6, 0, -11, 2,
    -13, -1, -13, 6,
    -1, -12, -1, -7,
    -4, -3, -4, 2,
    -6, -7, -8, 12,
    10, -13, 9, 12,
    2, 4, 3, 9,
    -2, 1, -2, 6,
    -1, -11, -2, 2,
    -10, -9, -5, 6,
    10, -2, 7, 3,
    6, -6, 8, -1,
    -11, -3, -6, -3,
    -9, 12, -4, 12,
    -6, -13, -9, 7,
    9, -10, 4, -9,
    4, -11, 0, 7,
    3, -4, 8, -4,
    -4, -8, -9, -4,
    -7, -4, -2, 11,
    -2, -9, 3, 11,
    0, 3, -4, 8,
    1, -12, 12, 8,
    0, -1, -13, 12,
    3, 0, -1, 5,
    -6, -10, 1, 8,
    0, -13, -13, 2,
    -2, -4, 12, 4,
    6, -13, -13, 9,
    -9, -10, 12, 11,
    8, -11, -6, 3,
    -4, -9, 10, 0,
    7, -3, -6, -1,
    8, 2, -6, 6,
    11, -8, -13, 3,
    -9, -6, 11, -4,
    -13, -1, 12, 1,
    -13, -11, -12, 12,
    7, -6, 9, 8,
    4, -6, 3, 4,
    -6, -11, -5, -6,
    -13, -13, -8, -13,
    0, 7, 0, 12,
    6, 3, 7, 8,
    2, -7, 2, -2,
    9, 4, 12, 9,
    10, -11, 11, -6,
    -3, -13, -5, 12,
    5, 0, 10, 2,
    -7, -4, -12, 4,
    -11, 2, -7, 7,
    12, -8, 5, 9,
    12, 6, 12, 12,
    1, -8, -1, 10,
    3, -13, 5, 7,
    -12, -13, -2, -2,
    -9, -2, -4, 1,
    -1, -5, -5, 12,
    -5, -1, -2, 4,
    7, -11, 2, 12,
    -3, -13, 2, -13,
    -2, -9, 1, -4,
    -7, -7, -2, -6,
    2, -7, 8, 5,
    -6, 8, -1, 10,
    -1, -7, -5, 5,
    1, 4, 6, 5,
    7, -3, 2, 7,
    0, -11, -5, -8,
    -2, 7, 3, 9,
    8, -7, 0, 2,
    0, -5, 5, -5,
    -13, -6, -1, 8,
    -2, -13, 7, 12,
    -10, -9, 2, 12,
    8, -13, -4, 12,
    -5, -4, 3, 8,
    -10, 10, 6, 10,
    -4, 1, 11, 9,
    -13, -1, 1, 3,
    -9, -11, 4, -3,
    3, -9, -9, 3,
    6, -6, -5, 10,
    -7, -8, 6, 8,
    -13, -4, 9, 9,
    9, -9, -8, 9,
    -10, -1, 6, 6,
    12, -4, -12, 8,
    -8, -5, 11, 6,
    7, -5, -10, 6,
    -7, 6, -9, 12,
    -11, -7, -8, -2,
    -4, -13, -3, 6,
    1, 0, 2, 5,
    12, -6, 9, -1,
    -1, -6, -1, -1,
    -7, -5, -7, 2,
    3, -13, 0, 0,
    6, -3, 8, 2,
    -4, -2, -5, 7,
    3, -4, 12, 12,
    -6, 0, -10, 8,
    -3, 4, -2, 9,
    1, -9, 2, 7,
    -10, -12, -9, -4,
    -10, -11, -5, -10,
    12, -4, 12, 11,
    8, -11, 12, 5,
    -3, 5, -6, 11,
    -8, -8, -13, -5,
    12, 4, 7, 5,
    -3, -5, -1, 7,
    -9, 5, -6, 11,
    11, -13, 2, 6,
    8, 1, 6, 6,
    5, -4, 4, 12,
    -4, -9, -13, 9,
    -5, -12, -1, 12,
    5, -9, 10, -5,
    12, 8, 7, 12,
    -2, -9, -4, -4,
    0, 8, 5, 12,
    1, -3, 4, 2,
    4, 6, 2, 11,
    8, -1, 3, 1,
    -1, -1, -3, 4,
    0, -5, 5, 9,
    -7, -12, -1, 2,
    -4, 12, 1, 12,
    -1, -13, 2, 2,
    4, -12, -1, -8,
    -3, -11, -8, 2,
    -6, 3, -1, 7,
    -2, 1, -7, 5,
    5, 3, 1, 8,
    12, -10, -1, -2,
    1, -4, -3, 1,
    0, -8, 5, 0,
    -3, -8, 2, 5,
    6, -7, 0, 10,
    -2, -8, 10, 12,
    12, -4, 1, 12,
    -6, -4, 0, -3,
    3, -2, -3, 12,
    12, 2, 0, 6,
    0, -6, -13, 5,
    -6, -13, 4, 11,
    4, -8, -3, 5,
    4, 5, -6, 12,
    -13, -8, 1, 0,
    3, -13, -10, -4,
    2, 6, -12, 7,
    8, -9, -4, -6,
    -3, -1, 6, 3,
    -11, -12, 4, 7,
    -2, -11, 8, 6,
    9, -1, -2, 2,
    -6, -12, 9, -6,
    2, -1, -7, 3,
    -8, -7, 3, 3,
    11, 5, -4, 9,
    9, -13, -13, -1,
    -8, -13, 9, 7,
    12, -2, -5, 12,
    7, 3, -13, 10,
    -13, -10, 12, 6,
    -8, 4, 8, 8,
    9, -5, -5, 5,
    -10, -10, 12, 0,
    -6, -6, 8, 3,
    -13, -6, 8, -2,
    12, 2, -9, 9,
    8, -8, -9, 0,
    -7, 0, 9, 4,
    -8, -3, 10, 0,
    9, -4, -12, 0,
    11, -10, 7, -4,
    10, -8, 8, 4,
    -10, -6, -10, 7,
    -1, -12, -1, 8,
    3, 0, 7, 12,
    -9, -13, -6, 10,
    -10, 7, -13, 12,
    -3, -12, -2, -2,
    -4, 0, -4, 12,
    -4, -6, -3, 0,
    8, -12, 7, -7,
    -11, -2, -8, 3,
    1, -9, 0, -4,
    5, -10, 3, -5,
    -2, -7, -1, 12,
    7, -8, 12, -8,
    8, -5, 6, 0,
    12, -1, 12, 4,
    -8, 6, -13, 8,
    -13, -8, -13, 3,
    -13, -12, -9, 2,
    5, -8, 5, -3,
    -10, -1, -10, 12,
    6, -9, 4, 8,
    -1, 2, 1, 8,
    -13, -5, -7, 10,
    -8, -8, -9, 0
};

ORBextractor::ORBextractor(int _nfeatures, float _scaleFactor, int _nlevels, int _scoreType,
         int _fastTh):
    nfeatures(_nfeatures), scaleFactor(_scaleFactor), nlevels(_nlevels),
    scoreType(_scoreType), fastTh(_fastTh)
{
    mvScaleFactor.resize(nlevels);
    mvScaleFactor[0]=1;
    for(int i=1; i<nlevels; i++)
        mvScaleFactor[i]=mvScaleFactor[i-1]*scaleFactor;

    float invScaleFactor = 1.0f/scaleFactor;
    mvInvScaleFactor.resize(nlevels);
    mvInvScaleFactor[0]=1;
    for(int i=1; i<nlevels; i++)
        mvInvScaleFactor[i]=mvInvScaleFactor[i-1]*invScaleFactor;

    mvImagePyramid.resize(nlevels);
    mvMaskPyramid.resize(nlevels);

    mnFeaturesPerLevel.resize(nlevels);
    float factor = (float)(1.0 / scaleFactor);
    float nDesiredFeaturesPerScale = nfeatures*(1 - factor)/(1 - (float)pow((double)factor, (double)nlevels));

    int sumFeatures = 0;
    for( int level = 0; level < nlevels-1; level++ )
    {
        mnFeaturesPerLevel[level] = cvRound(nDesiredFeaturesPerScale);
        sumFeatures += mnFeaturesPerLevel[level];
        nDesiredFeaturesPerScale *= factor;
    }
    mnFeaturesPerLevel[nlevels-1] = std::max(nfeatures - sumFeatures, 0);

    const int npoints = 512;
    const Point* pattern0 = (const Point*)bit_pattern_31_;
    std::copy(pattern0, pattern0 + npoints, std::back_inserter(pattern));

    //This is for orientation
    // pre-compute the end of a row in a circular patch
    umax.resize(CENTROID_RADIUS + 1);

    for (int i = 0; i <= CENTROID_RADIUS; i++) {
        double start = sqrt(CENTROID_RADIUS * CENTROID_RADIUS - i * i); // finds the y value that would make it smaller than a circle
        umax[i] = floor(start);
    }
}

static void computeOrientation(const Mat& image, vector<KeyPoint>& keypoints, const vector<int>& umax)
{
    for (vector<KeyPoint>::iterator keypoint = keypoints.begin(),
         keypointEnd = keypoints.end(); keypoint != keypointEnd; ++keypoint)
    {
        keypoint->angle = IC_Angle(image, keypoint->pt, umax);
    }
}

void ORBextractor::ComputeKeyPoints(vector<vector<KeyPoint> >& allKeypoints)
{

    allKeypoints.resize(nlevels);

    float imageRatio = (float)mvImagePyramid[0].cols/mvImagePyramid[0].rows;

    for (int level = 0; level < nlevels; ++level)
    {
        const int nDesiredFeatures = mnFeaturesPerLevel[level];

        const int levelCols = sqrt((float)nDesiredFeatures/(5*imageRatio));
        const int levelRows = imageRatio*levelCols;

        const int minBorderX = EDGE_THRESHOLD;
        const int minBorderY = minBorderX;
        const int maxBorderX = mvImagePyramid[level].cols-EDGE_THRESHOLD;
        const int maxBorderY = mvImagePyramid[level].rows-EDGE_THRESHOLD;

        const int W = maxBorderX - minBorderX;
        const int H = maxBorderY - minBorderY;
        const int cellW = ceil((float)W/levelCols);
        const int cellH = ceil((float)H/levelRows);

        const int nCells = levelRows*levelCols;
        const int nfeaturesCell = ceil((float)nDesiredFeatures/nCells);

        vector<vector<vector<KeyPoint> > > cellKeyPoints(levelRows, vector<vector<KeyPoint> >(levelCols));

        vector<vector<int> > nToRetain(levelRows,vector<int>(levelCols));
        vector<vector<int> > nTotal(levelRows,vector<int>(levelCols));
        vector<vector<bool> > bNoMore(levelRows,vector<bool>(levelCols,false));
        vector<int> iniXCol(levelCols);
        vector<int> iniYRow(levelRows);
        int nNoMore = 0;
        int nToDistribute = 0;


        float hY = cellH + 6;

        for(int i=0; i<levelRows; i++)
        {
            const float iniY = minBorderY + i*cellH - 3;
            iniYRow[i] = iniY;

            if(i == levelRows-1)
            {
                hY = maxBorderY+3-iniY;
                if(hY<=0)
                    continue;
            }
            
            float hX = cellW + 6;

            for(int j=0; j<levelCols; j++)
            {
                float iniX;

                if(i==0)
                {
                    iniX = minBorderX + j*cellW - 3;
                    iniXCol[j] = iniX;
                }
                else
                {
                    iniX = iniXCol[j];
                }


                if(j == levelCols-1)
                {
                    hX = maxBorderX+3-iniX;
                    if(hX<=0)
                        continue;
                }


                Mat cellImage = mvImagePyramid[level].rowRange(iniY,iniY+hY).colRange(iniX,iniX+hX);

                Mat cellMask;
                if(!mvMaskPyramid[level].empty())
                    cellMask = cv::Mat(mvMaskPyramid[level],Rect(iniX,iniY,hX,hY));

                cellKeyPoints[i][j].reserve(nfeaturesCell*5);

                learnedFast(cellImage,cellKeyPoints[i][j],fastTh,true);

                if(cellKeyPoints[i][j].size()<=3)
                {
                    cellKeyPoints[i][j].clear();

                    learnedFast(cellImage,cellKeyPoints[i][j],7,true);
                }

                if( scoreType == ORB::HARRIS_SCORE )
                {
                    // Compute the Harris cornerness
                    HarrisResponses(cellImage,cellKeyPoints[i][j], 7, HARRIS_K);
                }

                const int nKeys = cellKeyPoints[i][j].size();
                nTotal[i][j] = nKeys;

                if(nKeys>nfeaturesCell)
                {
                    nToRetain[i][j] = nfeaturesCell;
                    bNoMore[i][j] = false;
                }
                else
                {
                    nToRetain[i][j] = nKeys;
                    nToDistribute += nfeaturesCell-nKeys;
                    bNoMore[i][j] = true;
                    nNoMore++;
                }

            }
        }


        // Retain by score

        while(nToDistribute>0 && nNoMore<nCells)
        {
            int nNewFeaturesCell = nfeaturesCell + ceil((float)nToDistribute/(nCells-nNoMore));
            nToDistribute = 0;

            for(int i=0; i<levelRows; i++)
            {
                for(int j=0; j<levelCols; j++)
                {
                    if(!bNoMore[i][j])
                    {
                        if(nTotal[i][j]>nNewFeaturesCell)
                        {
                            nToRetain[i][j] = nNewFeaturesCell;
                            bNoMore[i][j] = false;
                        }
                        else
                        {
                            nToRetain[i][j] = nTotal[i][j];
                            nToDistribute += nNewFeaturesCell-nTotal[i][j];
                            bNoMore[i][j] = true;
                            nNoMore++;
                        }
                    }
                }
            }
        }

        vector<KeyPoint> & keypoints = allKeypoints[level];
        keypoints.reserve(nDesiredFeatures*2);

        const int scaledPatchSize = PATCH_SIZE*mvScaleFactor[level];

        // Retain by score and transform coordinates
        for(int i=0; i<levelRows; i++)
        {
            for(int j=0; j<levelCols; j++)
            {
                vector<KeyPoint> &keysCell = cellKeyPoints[i][j];
                KeyPointsFilter::retainBest(keysCell,nToRetain[i][j]);
                if((int)keysCell.size()>nToRetain[i][j])
                    keysCell.resize(nToRetain[i][j]);

                for(size_t k=0, kend=keysCell.size(); k<kend; k++)
                {
                    keysCell[k].pt.x+=iniXCol[j];
                    keysCell[k].pt.y+=iniYRow[i];
                    keysCell[k].octave=level;
                    keysCell[k].size = scaledPatchSize;
                    keypoints.push_back(keysCell[k]);
                }
            }
        }
        if((int)keypoints.size()>nDesiredFeatures)
        {
            KeyPointsFilter::retainBest(keypoints,nDesiredFeatures);
            keypoints.resize(nDesiredFeatures);
        }
    }

    // and compute orientations
    for (int level = 0; level < nlevels; ++level)
        computeOrientation(mvImagePyramid[level], allKeypoints[level], umax);
}

static void computeDescriptors(const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors,
                               const vector<Point>& pattern)
{
    descriptors = Mat::zeros((int)keypoints.size(), 32, CV_8UC1);

    for (size_t i = 0; i < keypoints.size(); i++)
        computeOrbDescriptor(keypoints[i], image, &pattern[0], descriptors.ptr((int)i));
}

void ORBextractor::operator()( InputArray _image, InputArray _mask, vector<KeyPoint>& _keypoints,
                      OutputArray _descriptors)
{ 
    if(_image.empty())
        return;

    Mat image = _image.getMat(), mask = _mask.getMat();
    assert(image.type() == CV_8UC1 );

    // Pre-compute the scale pyramids
    ComputePyramid(image, mask);

    vector < vector<KeyPoint> > allKeypoints;
    ComputeKeyPoints(allKeypoints);

    Mat descriptors;

    int nkeypoints = 0;
    for (int level = 0; level < nlevels; ++level)
        nkeypoints += (int)allKeypoints[level].size();
    if( nkeypoints == 0 )
        _descriptors.release();
    else
    {
        _descriptors.create(nkeypoints, 32, CV_8U);
        descriptors = _descriptors.getMat();
    }

    _keypoints.clear();
    _keypoints.reserve(nkeypoints);

    int offset = 0;
    for (int level = 0; level < nlevels; ++level)
    {
        vector<KeyPoint>& keypoints = allKeypoints[level];
        int nkeypointsLevel = (int)keypoints.size();

        if(nkeypointsLevel==0)
            continue;

        // preprocess the resized image
        Mat& workingMat = mvImagePyramid[level];
        GaussianBlur(workingMat, workingMat, Size(7, 7), 2, 2, BORDER_REFLECT_101);

        // Compute the descriptors
        Mat desc = descriptors.rowRange(offset, offset + nkeypointsLevel);
        computeDescriptors(workingMat, keypoints, desc, pattern);

        offset += nkeypointsLevel;

        // Scale keypoint coordinates
        if (level != 0)
        {
            float scale = mvScaleFactor[level]; //getScale(level, firstLevel, scaleFactor);
            for (vector<KeyPoint>::iterator keypoint = keypoints.begin(),
                 keypointEnd = keypoints.end(); keypoint != keypointEnd; ++keypoint)
                keypoint->pt *= scale;
        }
        // And add the keypoints to the output
        _keypoints.insert(_keypoints.end(), keypoints.begin(), keypoints.end());
    }
}

void ORBextractor::ComputePyramid(cv::Mat image, cv::Mat Mask)
{
    for (int level = 0; level < nlevels; ++level)
    {
        float scale = mvInvScaleFactor[level];
        Size sz(cvRound((float)image.cols*scale), cvRound((float)image.rows*scale));
        Size wholeSize(sz.width + EDGE_THRESHOLD*2, sz.height + EDGE_THRESHOLD*2);
        Mat temp(wholeSize, image.type()), masktemp;
        mvImagePyramid[level] = temp(Rect(EDGE_THRESHOLD, EDGE_THRESHOLD, sz.width, sz.height));

        if( !Mask.empty() )
        {
            masktemp = Mat(wholeSize, Mask.type());
            mvMaskPyramid[level] = masktemp(Rect(EDGE_THRESHOLD, EDGE_THRESHOLD, sz.width, sz.height));
        }

        // Compute the resized image
        if( level != 0 )
        {
            resize(mvImagePyramid[level-1], mvImagePyramid[level], sz, 0, 0, INTER_LINEAR);
            if (!Mask.empty())
            {
                resize(mvMaskPyramid[level-1], mvMaskPyramid[level], sz, 0, 0, INTER_NEAREST);
            }

            copyMakeBorder(mvImagePyramid[level], temp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                           BORDER_REFLECT_101+BORDER_ISOLATED);
            if (!Mask.empty())
                copyMakeBorder(mvMaskPyramid[level], masktemp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                               BORDER_CONSTANT+BORDER_ISOLATED);
        }
        else
        {
            copyMakeBorder(image, temp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                           BORDER_REFLECT_101);
            if( !Mask.empty() )
                copyMakeBorder(Mask, masktemp, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD, EDGE_THRESHOLD,
                               BORDER_CONSTANT+BORDER_ISOLATED);
        }
    }

}

} //namespace ORB_SLAM
