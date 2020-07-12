/**
 * g++ main.cpp corners.cpp -o main.o `pkg-config --cflags --libs opencv` -DDEBUG && ./main.o ../../data/img_23.jpg
 * 
 * This document is part of the project ScanIN. See License for more details.
 * This is alternative implementation of corner detection algorithm.
 * Author     : Satvik Choudhary
 * Created on : 12 July 2020
*/
#include <bits/stdc++.h>
#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "corners.hpp"

int main(int argc, char *argv[]) {
    std::string fileName = "img_33.jpg";
    std::string outName = "filtered.jpg";
    if (argc >= 2) {
        fileName = std::string (argv[1]);
        if (argc >= 3) {
            outName = argv[2];
        }
    }
    cv::Mat input = cv::imread(fileName, cv::IMREAD_COLOR);
    resize_image_if_bigger (input, 1024);
    
    std::vector <cv::Point> rect;
    int ret = find_best_corners (input, rect);

    cv::Mat dst;
    four_point_transform (input, dst, rect, cv::INTER_NEAREST);

    cv::namedWindow ("out", cv::WINDOW_AUTOSIZE);
    cv::imshow ("out", dst);
    cv::waitKey(0);
    cv::destroyAllWindows();

    std::cout << (ret == 0 ? "Perfect" : "Couldn't find") << std::endl;

    return 0;
}