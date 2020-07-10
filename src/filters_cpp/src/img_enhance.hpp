/**
 * Implements the dark magic filter code.
 * Tool for enhancing noisy scanned text images. The function of 'imgtxtenh' is to clean/
 * enhance noisy scanned text images, which could be printed or handwritten text. It takes as 
 * input an image and generates as output another image of the same size.
 * 
 * Its a C++ and opencv based implementation of 
 * @copyright Copyright (c) 2012-present, Mauricio Villegas <mauvilsa@upv.es>
 * @link https://github.cppom/mauvilsa/imgtxtenh
 * @license MIT License
 * 
 * This document is part of the project ScanIN. See License for more details.
 * 
 * Author     : Satvik Choudhary
 * Created on : 10 July 2020
*/

#ifndef __IMG_ENHANCE_HPP__
#define __IMG_ENHANCE_HPP__
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "intimg.hpp"

int dark_magic_filter_implementation (cv::Mat &src, cv::Mat &dst, int gb_enhtype=ENH_WOLF);

#endif