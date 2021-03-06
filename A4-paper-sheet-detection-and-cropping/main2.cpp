/*
#  File        : main.cpp
#  Description : Main procedure of A4 Paper Sheet Detection 
#                and Cropping with Hough Transform and Warping
#  Copyright   : HYPJUDY 2017/4/6
#  Details     : https://hypjudy.github.io/2017/03/28/cvpr-A4-paper-sheet-detection-and-cropping/
#  Code        : https://github.com/HYPJUDY/A4-paper-sheet-detection-and-cropping
*/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <bits/stdc++.h>
#include "Header_files/CImg.h"
#include "Warping.h"
using namespace std;

int main() {
	
    //reading the image
    vector <pair <int, int >> corner_points;
    cv::Mat image;
    image = cv::imread( "./dataset_original/3.jpg", CV_LOAD_IMAGE_COLOR);

	//for corner detection
    Hough hough(image , corner_points);

    //this is to show the points over the original image
    // cout << "Corner point ================== " << endl;
    // for (int i = 0 ; i < 4 ; i++)cout << corner_points[i].first << " " << corner_points[i].second << endl;
    // cout << " ========================" << endl;

    // //draw 4 points
    // cv::Mat original_image = cv::imread ("./dataset_original/3.jpg", CV_LOAD_IMAGE_COLOR);
    // for (int i= 0 ; i < 4 ; i++){
    //     cv::circle( original_image, cv::Point(corner_points[i].first , corner_points[i].second), 50, cv::Scalar( 0, 0, 255 ),cv::FILLED,cv::LINE_8  );
    // }
    
    // cv::imwrite ("./dataset_original/final_result.jpg" , original_image);
    

    hough.getMarkedImg().display();

    //for perspective transformation
	if (!hough.getError()){
        Warping Warping(hough);
        Warping.getCroppedImg().display() ;
    }
	return 0;
}

//To compile in c++ use
//g++ main2.cpp Warping.cpp Hough.cpp -I Header_files/eigen  `pkg-config --cflags --libs opencv` -pthread -lX11
