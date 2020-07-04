/**
 * Compile with g++ tophat.cpp -o tophat.o `pkg-config --cflags --libs opencv` && ./tophat.o
 * https://answers.opencv.org/question/66125/object-detection-in-nonuniform-illumination/#79578
*/
 
#include <bits/stdc++.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

using namespace std;
using namespace cv;
/** @brief Remove non-uniform illumination using morphology
Morphology OPEN can detects bright structures larger that a given size.
If you consider large structures as background you can use OPEN
to detect background than remove it from the original image.
This is same as to do MORPH_TOPHAT.
@Param [in]src input image GRAY, BGR or BGRA.
With BGR(A) image this function uses Brightness from image HSV.
@Param [out]dst destination image. If alpha channel is present in src it will be cloned in dst
@Param minThickess size used by morphology operation to estimate background. Use small size to
enhance details flatting larger structures.
@c minThickess should be just larger than maximum thickness in objects you want to keep.
Example:
- Take thickest object, suppose is circle 100 * 100px
- Measure its maximum thickness let's say is 20px: In this case @c minThickess could be 20+5.
- If the circle is filled than thickness=diameter, consequently @c minThickess should be 100+5px
@Param restoreMean if true, the mean of input brightness will be restored in destination image.
if false, the destination brightness will be close to darker region of input image.
@Param [out]background if not NULL the removed background will be returned here.
This will be Mat(src.size(),CV_8UC1)
*/

void NonUniformIlluminationMorph(const cv::Mat &src, cv::Mat &dst, int minThickess = 5, bool restoreMean = true, cv::Mat *background=NULL)
{
    CV_Assert(minThickess >= 0);
    CV_Assert((src.type() == CV_8UC1) || (src.type() == CV_8UC3) || (src.type() == CV_8UC4));
    cv::Mat brightness, src_hsv;
    std::vector<cv::Mat> hsv_planes;

    // GET THE BRIGHTNESS
    if (src.type() == CV_8UC1)
        src.copyTo(brightness);
    else if (src.type() == CV_8UC3)
    {
        cv::cvtColor(src, src_hsv, cv::COLOR_BGR2HSV);
        cv::split(src_hsv, hsv_planes);
        brightness = hsv_planes[2];
    }
    else if (src.type() == CV_8UC4)
    {
        cv::cvtColor(src, src_hsv, cv::COLOR_BGRA2BGR);
        cv::cvtColor(src_hsv, src_hsv, cv::COLOR_BGR2HSV);
        cv::split(src_hsv, hsv_planes);
        brightness = hsv_planes[2];
    }

    // to restore previous brightness we need its current mean
    Scalar m;
    if (restoreMean)
        m = mean(brightness);

    // REMOVE THE BACKGROUND
    int size = minThickess / 2;
    Point anchor = Point(size, size);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * size + 1, 2 * size + 1), anchor);
    if (background != NULL) // to keep background we need to use MORPH_OPEN
    {
        //get the background
        cv::Mat bkg(brightness.size(), CV_8UC1);
        morphologyEx(brightness, bkg, MORPH_OPEN, element, anchor);
        //save the background
        (*background) = bkg;
        //remove the background
        brightness = brightness - bkg;
    }
    else //tophat(I)  <=> open(I) - I; 
    {
        //remove background
        morphologyEx(brightness, brightness, MORPH_TOPHAT, element, anchor);
    }

    // RESTORE PREVIOUS BRIGHTNESS MEAN
    if (restoreMean)
        brightness += m(0);

    // BUILD THE DESTINATION
    if (src.type() == CV_8UC1)
        dst = brightness;
    else if (src.type() == CV_8UC3)
    {
        merge(hsv_planes, dst);
        cvtColor(dst, dst, COLOR_HSV2BGR);
    }
    // restore alpha channel from source 
    else if (src.type() == CV_8UC4)
    {
        cv::Mat bgr;
        vector<cv::Mat> bgr_planes = { hsv_planes[0], hsv_planes[1], hsv_planes[2]};
        merge(bgr_planes, bgr);
        cvtColor(bgr, bgr, COLOR_HSV2BGR);

        int from_toA[] = { 0, 0, 1, 1, 2, 2 };
        src.copyTo(dst);
        cv::mixChannels(&bgr, 1, &dst, 1, from_toA, 3);
    }

    imshow("NonUniformIlluminationMorph:SRC", src);
    imshow("NonUniformIlluminationMorph:DST", dst);
    if ((background != NULL) && (!background->empty()))
        imshow("NonUniformIlluminationMorph:BKG", *background);
}

int main (int argc, char *argv[]) {
    string filename = string("../../data/img_03.png");
    Mat image = imread( filename , CV_LOAD_IMAGE_COLOR);
    Mat dst;

    NonUniformIlluminationMorph(image, dst);
    waitKey(0);       
}