/**
 * This document is part of the project ScanIN. See License for more details.
 * This is alternative implementation of corner detection algorithm.
 * Author     : Satvik Choudhary
 * Created on : 12 July 2020
*/

#ifndef __CORNERS_HPP__
#define __CORNERS_HPP__
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

void resize_image_if_bigger (cv::Mat &input, const int dim, const int interpolation=cv::INTER_AREA);

int find_best_corners (cv::Mat &input, std::vector <cv::Point> &rect);

#endif