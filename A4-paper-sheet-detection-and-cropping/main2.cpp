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
	
     
    char *inPath = "./dataset/2.jpg";    
	cout << "Processing image : " << inPath << endl;
    
    cv::Mat image;
    image = cv::imread(inPath, CV_LOAD_IMAGE_COLOR);

    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", image );                   // Show our image inside it.

    cv::waitKey(0);

	//for corner detection
    Hough hough(image);

    //for perspective transformation
	Warping Warping(hough);
		
    //saving the results as image for corner detection
    string outPath = "./dataset/2";
	outPath +=  "_marked.jpg";
	char outPath2[100];
    strcpy(outPath2, outPath.c_str());
    hough.getMarkedImg().save(outPath2);

    //saving the results as image for edge detection
    outPath = "./dataset/2";
    outPath += "_A4.jpg";
	strcpy(outPath2, outPath.c_str());
	Warping.getCroppedImg().save(outPath2);

	return 0;
}
