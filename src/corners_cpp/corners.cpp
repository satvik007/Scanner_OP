/**
 * This document is part of the project ScanIN.
 * This is alternative implementation of corner detection algorithm.
 * Author     : Satvik Choudhary
 * Created on : 12 July 2020
*/

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Declaring parameters globally, found after experiments.
const int MORPH = 7;
const int CANNY_LOW = 3;
const int CANNY_HI = 20;
const int HOUGH = 30;
const int PI = acos (-1);
const int MIN_ANGLE = 60;
const int MAX_ANGLE = 120;

/**This code resizes the image if its width or height is bigger than necessary.
 * The aspect ratio is preserved.
 * This is done mainly to boost performance of the algorithms.
 * 
 * @param input 
 * The input image of type cv::Mat
 * 
 * @param dst
 * The output image of type cv::Mat
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
void resize_image_if_bigger (cv::Mat &input, cv::Mat &dst, const int dim, const int interpolation) {
    int maxdim = std::max(input.rows,input.cols);
    if (maxdim > dim)
    {
        double scale = (double)dim/(double)maxdim;
        cv::resize(input, dst, cv::Size(), scale, scale, interpolation);
    }
}

/**This function mainly does preprocessing on the image.
 * Applies Gaussian blur, median blur and dilates the image with a kernel based on 
 * MORPH_RECT on the image which is supposed to help with white on white detection.
 * 
 * @param img
 * The input image of type cv::Mat
 * 
 * @param dst
 * The output image of type cv::Mat
 * 
 * @param gauss_kernel
 * Size of the kernel for gaussian Blur, of type int (default=5)
 * 
 * @param median_kernel
 * Size of the kernel for median blur, of type int (default=9)
 * 
 * @param iterations
 * Number of times to apply the dilation operation, of type int (default=1)
*/
void _image_preprocessing (cv::Mat &img, cv::Mat &dst, const int gauss_kernel=5, const int median_kernel=9, const int iterations=1) {
    img.copyTo (dst);
    cv::GaussianBlur (dst, dst, cv::Size(gauss_kernel, gauss_kernel), 0);
    cv::medianBlur (dst, dst, median_kernel);
    // This helps in detection of white on white.
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(MORPH, MORPH));
    cv::dilate (dst, dst, kernel, cv::Point(-1, -1), iterations);
}

/**This function finds out the edges in the preprocessed image with the Canny Edge
 * Detector.
 * 
 * @param img
 * The input image of type cv::Mat
 * 
 * @param edges
 * Outputs image of type cv::Mat
 * 
*/
void _generate_edges (cv::Mat &img, cv::Mat &edges) {
    cv::Canny (img, edges, CANNY_LOW, CANNY_HI);
    std::vector <cv::Vec4i> lines;
    // Finding hough lines using the probablistic algorithm. This is mainly done so 
    // that we can extend lines if there are gaps in between.
    cv::HoughLinesP (edges, lines, 1, CV_PI/180, HOUGH);
    std::vector <cv::Vec4i>::iterator it = lines.begin();
#ifdef DEBUG        
    cv::Mat eg;
    img.copyTo (eg);
#endif
    // Drawing the lines found out by the previous algorithm, filling the gaps.
    for (; it != lines.end(); ++it) {
        cv::Vec4i l = *it;
        cv::line (edges, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, 8);
#ifdef DEBUG        
        cv::line (eg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 2, 8);
#endif
    }
    // Will close more gaps by dilating on MORPH_RECT based kernel.
    cv::Mat kernel = cv::getStructuringElement (cv::MORPH_RECT, cv::Size(MORPH, MORPH));
    cv::dilate (edges, edges, kernel, cv::Point(-1, -1), 1);
#ifdef DEBUG       
    cv::dilate (eg, eg, kernel, cv::Point(-1, -1), 1);
    cv::namedWindow ("eg", cv::WINDOW_AUTOSIZE);
    cv::imshow ("eg", eg);
    cv::waitKey(0);
    cv::destroyAllWindows();    
#endif
}

/**This function mainly removes 1 of the 2 points that are very close (< 100 pixels)
 * 
 * @param contour
 * Input contour of type std::vector < cv::Point >
 * 
 * @param res
 * Output contour of type std::vector < cv::Point >
*/
void _process_contours (std::vector < cv::Point > &contour, std::vector < cv::Point > &res) {
    std::vector < cv::Point > rect;
    int n = contour.size();
    bool last_modified = false;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        double dist = cv::norm (contour[i] - contour[j]);
        if (dist < 100) {
            if (i != n - 1 and !last_modified)
                rect.emplace_back (contour[j]);
            i++;
            last_modified = true;
        } else {
            rect.emplace_back (contour[i]);
            last_modified = false;
        }
    }
    res = rect;
}

/**This function finds out the angles A-O-B gives the three points and returns in degrees.
 * 
 * @param a
 * Input point of type cv::Point
 * 
 * @param b
 * Input point of type cv::Point
 * 
 * @param c
 * Input point of type cv::Point
 * 
 * @return theta 
 * Output angles in degrees of type double.
*/
double _angle (cv::Point a, cv::Point o, cv::Point b) {
    cv::Vec2f oa (cv::Point2f(o - a));
    cv::Vec2f ob (cv::Point2f(o - b));
    double v = oa.dot (ob) / (cv::norm (oa) * cv::norm (ob));
    v = std::max (-1.0, std::min (1.0, v));
    double theta = acos (v) * 180.0 / PI;
    return theta;
}

/**This function does a check on the contour see if all the angles are between
 * MIN_ANGLE and MAX_ANGLE, this weeds out some unnecessary contours.
 * 
 * @param a
 * Input contour of type std::vector < cv::Point >
 * 
 * @return bool
 * Whether the angle check succeeded or not.
*/
bool _angle_check (std::vector < cv::Point > &contour) {
    const int n = contour.size();
    if (n >= 3) {
        for (int i = 0; i < n; i++) {
            int j = (i - 1 + n) % n;
            int k = (i + 1) % n;
            double theta = _angle (contour[j], contour[i], contour[k]);
            if (theta < MIN_ANGLE or theta > MAX_ANGLE) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

/**This function finds out contours from the edges created by the Canny.
 * Then we weed out contours on the basis of arclength, area, angles
 * Returns the filtered contours in descending order sorted on basis of area.
 * 
 * @param edges
 * Input image with edges on black background of type cv::Mat
 * 
 * @param contours
 * Output contours found in the image of type std::vector< std::vector< cv::Point > >
 * 
 * @param cnt_method
 * The method to find contours. type int (default=cv::RETR_LIST)
 * 
 * – CV_RETR_EXTERNAL retrieves only the extreme outer contours.
 * It sets hierarchy[i][2]=hierarchy[i][3]=-1 for all the contours.
 * 
 * – CV_RETR_LIST retrieves all of the contours without establishing any hierarchical rela-
 * tionships.
 * 
 * – CV_RETR_CCOMP retrieves all of the contours and organizes them into a two-level
 * hierarchy. At the top level, there are external boundaries of the components. At the
 * second level, there are boundaries of the holes. If there is another contour inside a  
 * hole of a connected component, it is still put at the top level.
 * 
 * – CV_RETR_TREE retrieves all of the contours and reconstructs a full hierarchy of nested
 * contours. This full hierarchy is built and shown in the OpenCV contours.c demo.
 * method – Contour approximation method
 * 
 * – CV_CHAIN_APPROX_NONE stores absolutely all the contour points. That is, any 2
 * subsequent points (x1,y1) and (x2,y2) of the contour will be either horizontal, vertical
 * or diagonal neighbors, that is, max(abs(x1-x2),abs(y2-y1))==1 .
 * 
 * – CV_CHAIN_APPROX_SIMPLE compresses horizontal, vertical, and diagonal seg-
 * ments and leaves only their end points. For example, an up-right rectangular contour
 * is encoded with 4 points.
 *
 * – CV_CHAIN_APPROX_TC89_L1,CV_CHAIN_APPROX_TC89_KCOS applies one
 * of the flavors of the Teh-Chin chain approximation algorithm. See [TehChin89] for de-
 * tails.
*/
void _find_contours (cv::Mat edges, std::vector< std::vector< cv::Point > > &contours, const int cnt_method=cv::RETR_LIST) {

    std::vector< std::vector< cv::Point> > contoursCleaned;

    cv::findContours(edges, contours, cnt_method, cv::CHAIN_APPROX_TC89_KCOS);
    // clean up according to arclength and area.
    for (int i = 0; i < contours.size(); ++i) {
        if (cv::arcLength(contours[i], false) > 800 and cv::contourArea(contours[i]) > 20000) {
            contoursCleaned.push_back(contours[i]);
        }
    }
    
    contours.clear();
    for (int i = 0; i < contoursCleaned.size(); ++i) {
        std::vector< cv::Point> temp;
        // approximage contour to polygon.
        cv::approxPolyDP (cv::Mat (contoursCleaned[i]), contoursCleaned[i], 100, true);
        // find out convex Hull of the polygon.
        // Convex Hull is basically like wrapping a thread around the corner
        cv::convexHull (contoursCleaned[i], temp);
        // Remove points that are too close.
        _process_contours (temp, temp);
        // Apply angle check to weed out contours with angles outside of range.
        if (_angle_check(temp)) {
            contours.emplace_back (temp);
        }
    }
    // sort according to area in descending order.
    sort (contours.begin(), contours.end(), [&] (auto &a, auto &b) {
        return cv::contourArea (a) > cv::contourArea (b);
    });
}
 
/**This function ensures that the contour lies fully inside the image.
 * 
 * @param rect
 * Input contour of type std::vector < cv::Point >
 * 
 * @param size
 * Width and Height of image. of type cv::Size
*/
void _sanitize_rect (std::vector< cv::Point > &rect, cv::Size size) {
    if (rect.empty())  
        return;
    assert (rect.size() == 4);
    for (int i = 0; i < 4; i++) {
        rect[i].x = std::max (0, std::min (rect[i].x, size.width - 1));
        rect[i].y = std::max (0, std::min (rect[i].y, size.height - 1));
    }
}

/**This function returns the 4 corners of what the algorithm considers as the document.
 * 
 * @param inupt
 * Input image of type cv::Mat 
 * can be GRAY or BGR
 * 
 * @param rect
 * Output 4 points of the best shape found, type std::vector < cv::Point >
 * 
 * @return 
 * 0 if successful
 * -1 if default_rect is returned
*/
int find_best_corners (cv::Mat &input, std::vector < cv::Point > &rect) {
    cv::Mat img;
    std::vector < cv::Point > default_rect;
    default_rect.emplace_back (0, 0);
    default_rect.emplace_back (input.cols - 1, 0);
    default_rect.emplace_back (input.cols - 1, input.rows - 1);
    default_rect.emplace_back (0, input.rows - 1);

    // convert to gray or check if already gray.
    if (input.type() == CV_8UC3) {
        cv::cvtColor (input, img, cv::COLOR_BGR2GRAY);
    } else if (input.type() == CV_8UC1) {
        input.copyTo (img);
    } else {
        // input is neither gray nor BGR
        rect = default_rect;
        return -1;
    }

    _image_preprocessing (img, img, 5, 51, 2);

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

    _sanitize_rect (rect, cv::Size(input.cols, input.rows));

    if (rect.empty()) {
        rect = default_rect;
        return -1;
    }

    return 0;
}

/**This function reorders the points in clockwise with top left first.
 * 
 * @param pts
 * Input 4 points before reordering, type std::vector < cv::Point >
 * 
 * @param rect
 * Output 4 points after reordering, type std::vector < cv::Point >
 * 
 * @return 
 * 0 if successful
 * -1 if order_points failed.
*/
int order_points (std::vector < cv::Point > &pts, std::vector < cv::Point > &rect) {
    rect.resize (4);
    // top left
    rect[0] = *min_element (pts.begin(), pts.end(), [&] (auto &x, auto &y) {
        return x.x + x.y < y.x + y.y;
    });
    // bottom right
    rect[2] = *max_element (pts.begin(), pts.end(), [&] (auto &x, auto &y) {
        return x.x + x.y < y.x + y.y;
    });
    // top right
    rect[1] = *min_element (pts.begin(), pts.end(), [&] (auto &x, auto &y) {
        return x.y - x.x < y.y - y.x;
    });
    // bottom left
    rect[3] = *max_element (pts.begin(), pts.end(), [&] (auto &x, auto &y) {
        return x.y - x.x < y.y - y.x;
    });
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (rect[i] == rect[j]) {
                // Order points failed.
                // Take this seriously. Not terminating program to prevent app crash.
                return -1;
            }
        }
    }
    return 0;
}

/**This function validates if the points seem okay for a general document.
 * If it is assumed that the points are given in correct clockwise order we verify that.
 * Also we check if the points form a convex figure.
 * 
 * @param pts
 * Input 4 points before reordering, type std::vector < cv::Point >
 * 
 * @param given_in_order
 * If the points given are assumed to be in clockwise order, type is bool (default=False)
 * 
 * @return 
 * true if valid rect
 * false if invalid rect
*/
bool validate_points (std::vector < cv::Point > &pts, bool given_in_order) {
    assert (pts.size() == 4);
    std::vector < cv::Point > rect;
    order_points (pts, rect);

    if (given_in_order) {
        for (int i = 0; i < 4; i++) {
            if (pts[i] != rect[i]) {
                return false;
            }
        }
    }

    return cv::isContourConvex (rect);
}

/**This function crops out the shape given the four points and transforms the cropped
 *  image into a rectangle.
 * 
 * @param img
 * Input image of type cv::Mat
 * 
 * @param dst
 * Output image of type cv::Mat
 * 
 * @param pts
 * Input 4 points which describe area to be cropped, type std::vector < cv::Point >
 * 
 * @param flag
 * Type of interpolation to be used. type int (default=cv::INTER_NEAREST)
 * interpolation – interpolation method:
 * – INTER_NEAREST - a nearest-neighbor interpolation
 * – INTER_LINEAR - a bilinear interpolation 
 * – INTER_AREA - resampling using pixel area relation. It may be a preferred method for
 * image decimation, as it gives moire’-free results. But when the image is zoomed, 
 * it is similar to the INTER_NEAREST method. (used by default here)
 * – INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
 * – INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel neighborhood
 * 
 * @return 
 * 0 if successful
 * -1 if order_points failed.
*/
int four_point_transform (cv::Mat &img, cv::Mat &dst, std::vector < cv::Point > &pts, const int flag) {
    std::vector < cv::Point > pt_rect;
    int ret = order_points (pts, pt_rect);
    if (ret) {
        std::cerr << "Error while rotating points." << std::endl;
        img.copyTo (dst);
        return -1;
    }
    std::vector < cv::Point2f > rect;
    for (auto &el : pt_rect) {
        rect.emplace_back (el);
    }

    cv::Point tl, tr, br, bl;
    tl = rect[0];
    tr = rect[1];
    br = rect[2];
    bl = rect[3];
    double widthA = sqrt (pow(br.x - bl.x, 2) + pow(br.y - bl.y, 2));
    double widthB = sqrt (pow(tr.x - tl.x, 2) + pow(tr.y - tl.y, 2));
    int maxWidth = std::max ((int) widthA, (int) widthB);
    double heightA = sqrt (pow(tr.x - br.x, 2) + pow(tr.y - br.y, 2));
    double heightB = sqrt (pow(tl.x - bl.x, 2) + pow(tl.y - bl.y, 2));
    int maxHeight = std::max ((int) heightA, (int) heightB);

    std::vector < cv::Point2f > final_rect (4);
    final_rect[0] = cv::Point(0, 0);
    final_rect[1] = cv::Point(maxWidth - 1, 0);
    final_rect[2] = cv::Point(maxWidth - 1, maxHeight - 1);
    final_rect[3] = cv::Point(0, maxHeight - 1);
    // final_rect = rect;
    cv::Mat pt = cv::getPerspectiveTransform (rect, final_rect);
    cv::warpPerspective (img, dst, pt, cv::Size(maxWidth, maxHeight), flag);

    return 0;
}