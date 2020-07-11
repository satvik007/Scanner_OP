/**
 * This document is part of the project ScanIN.
 * This is alternative implementation of corner detection algorithm.
 * Author     : Satvik Choudhary
 * Created on : 12 July 2020
*/

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
// Declaring parameters globally, found after experiments.
const int MORPH = 9;
const int CANNY_LOW = 3;
const int CANNY_HI = 20;
const int HOUGH = 20;
const int PI = acos (-1);

/**This code resizes the image if its width is bigger than necessary.
 * This is done mainly to boost performance of the algorithms.
 * 
 * @param input 
 * The input image of type cv::Mat
 *  
 * @param dim
 * Maximum width of type int (1536 used by default)
 * 
 * @param interpolation
 * The interpolation algorithm type. Variable of type int.
 * interpolation – interpolation method:
 * – INTER_NEAREST - a nearest-neighbor interpolation
 * – INTER_LINEAR - a bilinear interpolation 
 * – INTER_AREA - resampling using pixel area relation. It may be a preferred method for
 *     image decimation, as it gives moire’-free results. But when the image is zoomed, it is
 *     similar to the INTER_NEAREST method. (used by default here)
 * – INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
 * – INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel neighborhood
*/
void resize_image_if_bigger (cv::Mat &input, const int dim, const int interpolation) {
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
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(MORPH, MORPH));
    cv::dilate (img, img, kernel, cv::Point(-1, -1), iterations);
}

void _generate_edges (cv::Mat &img, cv::Mat &edges) {
    cv::Canny (img, edges, CANNY_LOW, CANNY_HI);
    std::vector <cv::Vec4i> lines;
    cv::HoughLinesP (edges, lines, 1, CV_PI/180, HOUGH);
    std::vector <cv::Vec4i>::iterator it = lines.begin();
    for (; it != lines.end(); ++it) {
        cv::Vec4i l = *it;
        cv::line (edges, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, 8);
    }
}

void _find_contours (cv::Mat edges, std::vector< std::vector< cv::Point > > &contours, const int cnt_method=CV_RETR_LIST) {
    std::vector< std::vector< cv::Point> > contoursCleaned, contoursArea;
    cv::findContours(edges, contours, cnt_method, CV_CHAIN_APPROX_TC89_KCOS);
    // clean up according to arclength
    for (int i = 0; i < contours.size(); ++i) {
        if (cv::arcLength(contours[i], false) > 400)
            contoursCleaned.push_back(contours[i]);
    }
    // clean up according to area
    for (int i = 0; i < contoursCleaned.size(); ++i) {
        if (cv::contourArea(contoursCleaned[i]) > 10000){
            contoursArea.push_back(contoursCleaned[i]);
        }
    }
    // approximage contour to polygon.
    contours.resize (contoursArea.size());
    for (int i = 0; i < contoursArea.size(); ++i) {
        cv::approxPolyDP (cv::Mat (contoursArea[i]), contours[i], 100, true);
    }
    // sort according to area in descending order.
    sort (contours.begin(), contours.end(), [&] (auto &a, auto &b) {
        return cv::contourArea (a) > cv::contourArea (b);
    });
}
 
void _sanitize_rect (std::vector< cv::Point > &rect, cv::Size size) {
    if (rect.empty())  
        return;
    assert (rect.size() == 4);
    for (int i = 0; i < 4; i++) {
        rect[i].x = std::max (0, std::min (rect[i].x, size.height - 1));
        rect[i].y = std::max (0, std::min (rect[i].y, size.width - 1));
    }
}

int find_best_corners (cv::Mat &input, std::vector < cv::Point > &rect) {
    cv::Mat img;
    std::vector < cv::Point > default_rect;
    default_rect.emplace_back (0, 0);
    default_rect.emplace_back (0, input.cols - 1);
    default_rect.emplace_back (input.rows - 1, input.cols - 1);
    default_rect.emplace_back (input.rows - 1, 0);

    // convert to gray
    if (input.type() == CV_8UC3) {
        cv::cvtColor (input, img, cv::COLOR_BGR2GRAY);
    } else if (input.type() == CV_8UC1) {
        input.copyTo (img);
    } else {
        // input is neither gray nor BGR
        rect = default_rect;
        return -1;
    }

    _image_preprocessing (img, 5, 51, 2);

#ifdef DEBUG
    cv::namedWindow ("blur", cv::WINDOW_AUTOSIZE);
    cv::imshow ("blur", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    cv::Mat edges;
    _generate_edges (img, edges);

#ifdef DEBUG
    cv::namedWindow ("edges", cv::WINDOW_AUTOSIZE);
    cv::imshow ("edges", edges);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif

    std::vector< std::vector< cv::Point > > contours;
    _find_contours (edges, contours);
    
 #ifdef DEBUG
    cv::Mat boxes;
    input.copyTo (boxes);
    cv::polylines (boxes, contours, true, cv::Scalar(0, 0, 255), 3);
    cv::namedWindow ("boxes", cv::WINDOW_AUTOSIZE);
    cv::imshow ("boxes", boxes);
    cv::waitKey(0);
    cv::destroyAllWindows();
#endif   

    for (int i = 0; i < contours.size(); i++) {
        if (contours[i].size() == 4) {
            rect = contours[i];
            break;
        }
    }

    _sanitize_rect (rect, cv::Size(input.rows, input.cols));

    if (rect.empty()) {
        rect = default_rect;
        return -1;
    }

    return 0;
}
