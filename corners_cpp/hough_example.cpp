/**
 * g++ hough_example.cpp -o hough_example.o `pkg-config --cflags --libs opencv` && 
 * ./hough_example.o ../../data/img_23.jpg
*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void houghThreshold(int, void*)
{
  

  src.copyTo( dst, detected_edges);
  imshow( window_name, dst );
}

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
    cv::GaussianBlur (img, img, cv::Size(gauss_kernel, gauss_kernel), 0);
    cv::medianBlur (img, img, median_kernel);
    // This helps in detection of white on white.
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(9, 9));
    cv::dilate (img, img, kernel, cv::Point(-1, -1), iterations);
}

/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1] );

  _resize_image_if_bigger (src, 1024);

  _image_preprocessing (src, 5, 51);

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Canny detector
  Canny( src_gray, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  dst = Scalar::all(0);

  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, houghThreshold );

  /// Show the image
  houghThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }