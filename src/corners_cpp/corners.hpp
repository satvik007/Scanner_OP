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

void resize_image_if_bigger (cv::Mat &input, cv::Mat &dst, const int dim=1536, const int interpolation=cv::INTER_AREA);

int find_best_corners (cv::Mat &input, std::vector <cv::Point> &rect);

int order_points (std::vector < cv::Point > &pts, std::vector < cv::Point > &rect);

int four_point_transform (cv::Mat &img, cv::Mat &dst, std::vector < cv::Point > &pts, const int flag=cv::INTER_NEAREST);

bool validate_points (std::vector < cv::Point > &pts, bool given_in_order=false);

#endif