/**
 * g++ canny_example.cpp -o canny_example.o `pkg-config --cflags --libs opencv` && 
 * ./canny_example.o ../../data/img_23.jpg
*/ 
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;

/// Global variables

Mat src, src_gray, temp1, temp2, edges;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 25;
int maxThreshold= 60;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";
int blurSize = 9;
int medianBlurSize = 7;
int morphSize = 4;
int iterations = 1;
int sigmaColor = 10;
int sigmaSpace = 75;
int hough = 30;
int minGap = 30;
int maxGap = 10;
int rho = 1;

void _resize_image_if_bigger (cv::Mat &input, const int dim, const int interpolation=cv::INTER_AREA) {
    int maxdim = input.cols; //std::max(input.rows,input.cols);
    if (maxdim > dim)
    {
        double scale = (double)dim/(double)maxdim;
        cv::Mat t;
        cv::resize(input, t, cv::Size(), scale, scale, interpolation);
        input = t;
    }
}

void _image_preprocessing (cv::Mat &img, const int gauss_kernel=5, const int median_kernel=9, const int iterations=1) {
    // cv::GaussianBlur (img, img, cv::Size(gauss_kernel, gauss_kernel), 1);
    cv::medianBlur (img, img, median_kernel);
    // This helps in detection of white on white.
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(morphSize*2+1, morphSize*2+1));
    cv::dilate (img, img, kernel, cv::Point(-1, -1), iterations);
    cv::erode (img, img, kernel, cv::Point(-1, -1), iterations);
}

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{  
  /// Reduce noise with a kernel 3x3
//   blur( src_gray, detected_edges, Size(2 * blurSize + 1, 2 * blurSize + 1) );

  cv::cvtColor (src, temp1, cv::COLOR_BGR2Lab);
  cv::bilateralFilter (temp1, temp2, 2 * blurSize + 1, sigmaColor, sigmaSpace);
  cv::cvtColor (temp2, temp2, cv::COLOR_Lab2BGR);
  
  _image_preprocessing (temp2, 2 * blurSize + 1, 2 * medianBlurSize + 1, iterations);

  imshow ("medianBlur", temp2);

  /// Canny detector
  Canny( temp2, detected_edges, lowThreshold, maxThreshold, kernel_size );

  std::vector <cv::Vec4i> lines;

  cv::HoughLinesP (detected_edges, lines, rho, CV_PI/180, hough, minGap, maxGap);
  std::vector <cv::Vec4i>::iterator it = lines.begin();

  src.copyTo (edges);

  // Drawing the lines found out by the previous algorithm, filling the gaps.
  for (; it != lines.end(); ++it) {
      cv::Vec4i l = *it;
      cv::line (edges, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, 8);
  }
  // Will close more gaps by dilating on MORPH_RECT based kernel.
  //cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(morphSize * 2 + 1, ////morphSize * 2 + 1));
  //cv::dilate (edges, edges, kernel, cv::Point(-1, -1), 1);

  cv::imshow ("edges", edges);

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
  imshow( window_name, dst );
 }


/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1] , cv::IMREAD_COLOR);

  _resize_image_if_bigger (src, 600);

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  namedWindow ( "edges", cv::WINDOW_AUTOSIZE);  

  /// Convert the image to grayscale
  cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, cv::WINDOW_AUTOSIZE );
  namedWindow( "medianBlur", cv::WINDOW_AUTOSIZE );

  /// Create a Trackbar for user to enter threshold
  //createTrackbar( "Min Threshold:", window_name, &lowThreshold, 200, CannyThreshold );
  //createTrackbar( "Max Threshold:", window_name, &maxThreshold, 200, CannyThreshold);
  createTrackbar( "Blur_Size:", window_name, &blurSize, max_lowThreshold, CannyThreshold);
  createTrackbar( "Median_Blur_Size:", window_name, &medianBlurSize, max_lowThreshold, CannyThreshold);
  // createTrackbar( "Morph_Size:", window_name, &morphSize, max_lowThreshold, CannyThreshold);
  // createTrackbar( "Morph_iter:", window_name, &iterations, max_lowThreshold, CannyThreshold);
  createTrackbar( "Threshold:", window_name, &hough, 100, CannyThreshold);
  createTrackbar( "MinGap:", window_name, &minGap, 100, CannyThreshold);
  createTrackbar( "MaxGap:", window_name, &maxGap, 100, CannyThreshold);

  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);
  cv::destroyAllWindows();
  return 0;
  }