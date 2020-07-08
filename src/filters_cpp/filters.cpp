/**
 * This document is part of the project ScanIN. See License for more details.
 * This is implementation of all the filters of the app, it contains the following filters
 *  - Magic
 *  - Sepia
 *  - Lighten
 *  - Gray 
 *  - Sharpen
 * Author     : Satvik Choudhary
 * Created on : 8 July 2020
*/

#ifndef __FILTERS_HPP__
#define __FILTERS_HPP__
#include <opencv2/core/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/imgproc.hpp>
#include "filters.hpp"
#include <vector>

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
    if ( maxdim > dim )
    {
        double scale = (double)dim/(double)maxdim;
        cv::Mat t;
        cv::resize( input, t, cv::Size(), scale, scale, interpolation);
        input = t;
    }
}

/**This code sharpens the image using an unsharp mask, a popular functionality in PhotoShop
 *
 * @param src
 * The input image of type cv::Mat
 * 
 * @param dst
 * Destination image of type cv::Mat
 *  
 * @param kernel_size
 * Size of kernel used for Gaussian Blur. Type=cv::Size (default=(5, 5))
 * 
 * @param sigma
 * Gaussian kernel standard deviation. More sigma -> more blur, type=double (default=1.0)
 * 
 * @param amount
 * will increase sharpness, type=double (default=1.0)
 * 
 * @param threshold
 * take pixel from original image if (image - blurred) < threshold, type=int (default=0)
*/ 
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
        // where temp < threshold use the original pixel value.
        src.copyTo (dst, low_contrast_mask);
    }
}

/**This code implements the magic filter which is based on sharpening of image followed by
 * increasing the brightness.
 * 
 * @param src
 * The input image of type cv::Mat
 * 
 * @param dst
 * Destination image of type cv::Mat
 * 
 * @param alpha
 * Constant multiplies by image to increase contrast, type double (default=1.5)
 * 
 * @param beta
 * Constant subtracted from the result of multiplying, type int (default=0)
 * 
 * @param threshold
 * For pixels with very low values, get the original value back if its less than threshold. 
 * Type int (default=0)
*/
void magic_filter (cv::Mat &src, cv::Mat &dst, const double alpha, const int beta, const int threshold) {
    // apply sharpness filter
    _unsharp_mask (src, dst);
    // increase contrast
    src.convertTo (dst, -1, alpha, beta);
    if (threshold > 0) {
        cv::Mat mask = src < threshold;
        src.copyTo (dst, mask);
    }
};

/**This code implements sepia, a popular filter from instagram.
 * @param src
 * The input image of type cv::Mat
 * 
 * @param dst
 * Destination image of type cv::Mat
*/ 
void sepia_filter (cv::Mat &src, cv::Mat &dst) {
    std::vector <cv::Mat> bgr;
    cv::split(src, bgr);
    cv::Mat r_new, g_new, b_new;
    r_new = bgr[2] * 0.393 + bgr[1] * 0.769 + bgr[0] * 0.189;
    g_new = bgr[2] * 0.349 + bgr[1] * 0.686 + bgr[0] * 0.168;
    b_new = bgr[2] * 0.272 + bgr[1] * 0.534 + bgr[0] * 0.131;
    merge (std::vector <cv::Mat> ({b_new, g_new, r_new}), dst);
};

/**This code implements lighten filter, which just increases the brightness of the image.
 * @param src
 * The input image of type cv::Mat
 * 
 * @param dst
 * Destination image of type cv::Mat
*/ 
void lighten_filter (cv::Mat &src, cv::Mat &dst) {
    src.convertTo (dst, -1, 1.0, 20);
};

void gray_filter (cv::Mat &src, cv::Mat &dst) {
    // To be handled after refactoring imgtxtenh

};

/**This code implements sharpen filter, which applies the _unsharp_mask to make the image sharper
 * @param src
 * The input image of type cv::Mat
 * 
 * @param dst
 * Destination image of type cv::Mat
*/ 
void sharpen_filter (cv::Mat &src, cv::Mat &dst, cv::Size kernel_size, const double sigma, const double amount, const int threshold) {
    _unsharp_mask (src, dst, kernel_size, sigma, amount, threshold);
};

#endif