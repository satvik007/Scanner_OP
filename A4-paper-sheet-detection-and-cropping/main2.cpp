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
#include <bits/stdc++.h>
#include "Header_files/CImg.h"
#include "Warping.h"
using namespace std;

int main() {
	
     
    char *inPath = "./dataset_original/4.jpg";    
	cout << "Processing image : " << inPath << endl;
    vector <pair <int, int >> corner_points;
    cv::Mat image;
    image = cv::imread(inPath, CV_LOAD_IMAGE_COLOR);

	//for corner detection
    Hough hough(image , corner_points);

    cout << "Corner point ================== " << endl;
    for (int i = 0 ; i < 4 ; i++)cout << corner_points[i].first << " " << corner_points[i].second << endl;
    cout << " ========================" << endl;

    hough.getMarkedImg().display();

    //for perspective transformation
	if (!hough.getError()){
        Warping Warping(hough);
        Warping.getCroppedImg().display() ;
    }
	return 0;
}
