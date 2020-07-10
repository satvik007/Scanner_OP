#include <bits/stdc++.h>
#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

#include "filters.hpp"

int main(int argc, char *argv[]) {
    std::string fileName = "img_16.jpg";
    std::string outName = "filtered.jpg";
    if (argc >= 2) {
        fileName = std::string (argv[1]);
        if (argc >= 3) {
            outName = argv[2];
        }
    }
    cv::Mat input = cv::imread(fileName, cv::IMREAD_COLOR);
    resize_image_if_bigger (input, 1536);
    cv::Mat dst;
    // magic_filter (input, dst, 1.4, -50);
    dark_magic_filter (input, dst);

    imwrite (outName, dst);
}