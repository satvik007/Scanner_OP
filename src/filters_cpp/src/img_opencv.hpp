/**
 * Implements some of the opencv related functions.
 * 
 * This document is part of the project ScanIN. See License for more details.
 * This is implementation of 
 * Author     : Satvik Choudhary
 * Created on : 10 July 2020
*/

#ifndef __IMG_OPENCV_HPP__
#define __IMG_OPENCV_HPP__
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "mem.hpp"

int getpixels_opencv_graym (cv::Mat &img, gray** gimg);
int writeimg_opencv (gray **gimg, cv::Mat &img);

#endif