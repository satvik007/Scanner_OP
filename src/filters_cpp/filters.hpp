/**
 * This document is part of the project ScanIN. See License for more details.
 * This is implementation of all the filters of the app, it contains the following filters
 *  - Magic filter
 *  - Sepia
 *  - Lighten
 *  - Gray 
 *  - Black/White
 * Author     : Satvik Choudhary
 * Created on : 8 July 2020
*/

#ifndef __FILTERS_HPP__
#define __FILTERS_HPP__
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

void resize_image_if_bigger (cv::Mat &input, const int dim=1536, const int interpolation=CV_INTER_AREA);

void magic_filter (cv::Mat &src, cv::Mat &dst, const double alpha=1.5, const int beta=0) {};

void sepia_filter (cv::Mat &src, cv::Mat &dst) {};

void lighten_filter (cv::Mat &src, cv::Mat &dst) {};

void gray_filter (cv::Mat &src, cv::Mat &dst) {};

void sharpen_filter (cv::Mat &src, cv::Mat &dst) {};

#endif