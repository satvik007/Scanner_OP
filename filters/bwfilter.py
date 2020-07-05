# Usage: 

# python bwfilter.py --input=./data/test1.jpg 

import cv2
import numpy as np
import argparse


def parse_args():
    parser = argparse.ArgumentParser(add_help=True, description='testing B&W filter')
    required_named = parser.add_argument_group('required named arguments')
    required_named.add_argument('-i', '--input', type=str, help='path of the input image', required=True)
    return parser.parse_args()


def show_img(img):
    cv2.namedWindow("output", cv2.WINDOW_NORMAL)
    cv2.resizeWindow('output', 900, 900)
    cv2.imshow("output", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def bwfilter(bwimg):
    # blur the image
    bwimg = cv2.GaussianBlur(bwimg,(7,7),7)
    cv2.imwrite('blur.png', bwimg)

    bwimg = cv2.bilateralFilter(bwimg,9,75,75)
    cv2.imwrite('bilat.png', bwimg)

    # adaptive threshholding 
    bwimg = cv2.adaptiveThreshold(bwimg,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY,11,2)
    cv2.imwrite('adaptth.png', bwimg)

    return bwimg


if __name__=='__main__':
    args = parse_args()
    img = cv2.imread(args.input, 0)
    bwimg = bwfilter(img)
    show_img(bwimg)
    cv2.imwrite('bwimg.png', bwimg)