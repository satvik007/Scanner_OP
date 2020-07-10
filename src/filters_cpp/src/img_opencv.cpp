/**
 * Implements some of the opencv related functions.
 * 
 * This document is part of the project ScanIN. See License for more details.
 * This is implementation of 
 * Author     : Satvik Choudhary
 * Created on : 10 July 2020
*/

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "img_opencv.hpp"
#include "mem.hpp"

int getpixels_opencv_graym (cv::Mat &img, gray** gimg) {
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            gimg[i][j] = img.at<uint8_t> (i, j);
        }
    }
    return 0;
}

int writeimg_opencv (gray **gimg, cv::Mat &img) {
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            img.at<uint8_t> (i, j) = gimg[i][j];
        }
    }
    return 0;
}