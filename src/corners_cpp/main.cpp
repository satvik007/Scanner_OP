/**
 * g++ main.cpp corners.cpp -o main.o `pkg-config --cflags --libs opencv4` -DDEBUG && ./main.o ../../data/img_23.jpg
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
#include "dirent.h"
#include <string> 

int main(int argc, char *argv[]) {
#ifdef DEBUG
    // cv::namedWindow ("out", cv::WINDOW_AUTOSIZE);
#endif

    if (argc == 1) {
        DIR *dir;
        struct dirent *ent;
        std::string dirName = "/home/satvik/codes/Scanner_OP/dataset";

        if ((dir = opendir (dirName.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                std::string fileName = ent->d_name;
                if (fileName.find(".jpg") != std::string::npos) {
                    std::cout << fileName << std::endl;

                    cv::Mat input = cv::imread(dirName + fileName, cv::IMREAD_COLOR);
                    resize_image_if_bigger (input, input, 1500);
    #ifdef DEBUG
                    // cv::imshow ("out", input);
                    // cv::waitKey(0);
    #endif
                    std::vector <cv::Point> rect;
                    int ret = find_best_corners (input, rect);
                }
        
            }
            closedir (dir);
        } else {
            perror ("");
            return EXIT_FAILURE;
        }
    } else {
        std::string fileName = std::string (argv[1]);
        cv::Mat input = cv::imread(fileName, cv::IMREAD_COLOR);
                    resize_image_if_bigger (input, input, 1500);
 
        std::vector <cv::Point> rect;
        int ret = find_best_corners (input, rect);
}

    

#ifdef DEBUG
    cv::destroyAllWindows();
#endif    


    // cv::imwrite (outName, dst);

    return 0;
}