/*
Compile with 
g++ filter2.cpp -o filter2.o `pkg-config --cflags --libs opencv` && ./filter2.o [filePath]
[] - optional
https://stackoverflow.com/questions/49997681/scanned-document-text-background-clarity-not-good-using-opencv-ios
*/
#include <bits/stdc++.h>
#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

using namespace std;
using namespace cv;

std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1); 
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        { 
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height) 
                boundRect.push_back(appRect);
        }
    return boundRect;
}

void resize_image (cv::Mat &input, const int dim=1536) {
    int maxdim = input.cols; //std::max(input.rows,input.cols);
    //const int dim = 1536;
    if ( maxdim > dim )
    {
        double scale = (double)dim/(double)maxdim;
        cv::Mat t;
        cv::resize( input, t, cv::Size(), scale, scale );
        input = t;
    }
}

void _unsharp_mask (cv::Mat &src, cv::Mat &dst, cv::Size kernel_size=cv::Size(5, 5), const double sigma=1.0, const double amount=1.0, const int threshold=0) {
    cv::Mat blur;
    cv::GaussianBlur (src, blur, kernel_size, sigma);
    // dst = src * (amount + 1.0)
    src.convertTo (dst, CV_64FC3, amount + 1.0, 0);
    cv::Mat temp;
    // temp = blur * (amount)
    blur.convertTo (temp, CV_64FC3, amount, 0);
    // dst = src * (amount + 1.0) - blur * (amount)
    cv::subtract (dst, temp, dst, cv::noArray(), -1);
    dst.convertTo (dst, CV_8UC3);
    if (threshold > 0) {
        cv::Mat low_contrast_mask;
        // temp = abs(src - blur)
        cv::absdiff (src, blur, temp);
        low_contrast_mask = temp < threshold;
        src.copyTo (dst, low_contrast_mask);
    }
}

int applyOldMagicFilter (cv::Mat &input, cv::Mat &dst) {
    
    if ( input.type()!=CV_8UC3 )
        CV_Error(CV_StsError,"!bgr");

    cv::Mat result;
    input.copyTo( result ); // result is just for drawing the text rectangles
    cv::Mat median;
    // remove highlight pixels e.g., those from debayer-artefacts and noise
    cv::medianBlur(input,median,5);
    cv::Mat localmax;
    // find local maximum
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(15,15) );
    cv::morphologyEx( median,localmax,cv::MORPH_CLOSE,kernel,cv::Point(-1,-1),1,cv::BORDER_REFLECT101 );

    // detectLetters by @William, modified to internally do the grayscale conversion if necessary
    // https://stackoverflow.com/questions/23506105/extracting-text-opencv?rq=1
    std::vector< cv::Rect > bb = detectLetters( input);

    // compose a simple Gaussian model for text background (still assumed white)
    cv::Mat mask( input.size(),CV_8UC1,cv::Scalar( 0 ) );
    if ( bb.empty() )
        return 0; // TODO; none found
    for ( size_t i=0;i<bb.size(); ++i )
    {
        cv::rectangle( result, bb[i], cv::Scalar(0,0,255),2,8 ); // visualize only
        cv::rectangle( mask, bb[i], cv::Scalar( 1 ), -1 ); // create a mask for cv::meanStdDev 
    }
    cv::Mat mean,dev;
    cv::meanStdDev( localmax, mean, dev, mask );
    if ( mean.type()!=CV_64FC1 || dev.type()!=CV_64FC1 || mean.size()!=cv::Size(1,3) || dev.size()!=cv::Size(1,3) )
        CV_Error(CV_StsError, "should never happen");
    double minimum[3];
    double maximum[3];
    // simply truncate the localmax according to our simple Gaussian model (+/- one standard deviation)
    for ( unsigned int u=0;u<3;++u )
    {
        minimum[u] = mean.at<double>(u ) - dev.at<double>( u );
        maximum[u] = mean.at<double>(u ) + dev.at<double>( u );
    }
    for ( int y=0;y<mask.rows;++y){
        for ( int x=0;x<mask.cols;++x){
            cv::Vec3b & col = localmax.at<cv::Vec3b>(y,x);
            for ( unsigned int u=0;u<3;++u )
            {
                if ( col[u]>maximum[u] )
                    col[u]=maximum[u];
                else if ( col[u]<minimum[u] )
                    col[u]=minimum[u];
            }
        }
    }
    // do the per pixel gain then
    input.copyTo( dst );
    for ( int y=0;y<input.rows;++y){
        for ( int x=0;x<input.cols;++x){
            const cv::Vec3b & v1=input.at<cv::Vec3b>(y,x);
            const cv::Vec3b & v2=localmax.at<cv::Vec3b>(y,x);
            cv::Vec3b & v3=dst.at<cv::Vec3b>(y,x);
            for ( int i=0;i<3;++i )
            {
                double gain = 255.0/(double)v2[i];
                v3[i] = cv::saturate_cast<unsigned char>( gain * v1[i] );
            }
        }
    }
    return 0;
}



int main(int argc, char *argv[]) {
    string fileName = "../../data/img_16.jpg";
    string outName = "images/filtered2.jpg";
    if (argc >= 2) {
        fileName = string (argv[1]);
        if (argc >= 3) {
            outName = argv[2];
        }
    }
    cv::Mat input = cv::imread(fileName, CV_LOAD_IMAGE_COLOR );
    // reduce dim for speed.
    resize_image (input, 1536);
    cv::Mat dst;
    // applyOldMagicFilter (input, dst);
    _unsharp_mask (input, dst, cv::Size(5, 5), 1.0, 1.0, 1);
    namedWindow ("mask1", WINDOW_AUTOSIZE);
    imshow ("mask1", dst);
    waitKey(0);
    _unsharp_mask (input, dst, cv::Size(5, 5), 1.0, 1.0, 20);
    namedWindow ("mask2", WINDOW_AUTOSIZE);
    imshow ("mask2", dst);
    waitKey(0);

    imwrite (outName, dst);
}
   