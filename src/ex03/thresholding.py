import cv2, numpy as np
import sys

if __name__ == '__main__':
    # read image
    orig = cv2.imread(sys.argv[1])

    # grey scaling
    grey_img = cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY)

    # global thresholding
    ret, global_thresh = cv2.threshold(grey_img, 100, 255, cv2.THRESH_BINARY)

    # adaptive thresholding
    adapt_thresh = cv2.adaptiveThreshold(grey_img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)

    # denoishing
    denoised = cv2.fastNlMeansDenoising(adapt_thresh, 11, 31, 9)

    # cv2.namedWindow('result', cv2.WINDOW_NORMAL)
    cv2.imshow('orig', orig)
    cv2.waitKey(0)
    cv2.imshow('grey_img', grey_img)
    cv2.waitKey(0)
    cv2.imshow('global_thresh', global_thresh)
    cv2.waitKey(0)
    cv2.imshow('adapt_thresh', adapt_thresh)
    cv2.waitKey(0)
    cv2.imshow('denoised', denoised)
    cv2.waitKey(0)

    cv2.destroyAllWindows()

    cv2.imwrite('src/ex03/images/'+'orig'+'.jpg', orig)
    cv2.imwrite('src/ex03/images/'+'grey_img'+'.jpg', grey_img)
    cv2.imwrite('src/ex03/images/'+'global_thresh'+'.jpg', global_thresh)
    cv2.imwrite('src/ex03/images/'+'adapt_thresh'+'.jpg', adapt_thresh)
    cv2.imwrite('src/ex03/images/'+'denoised'+'.jpg', denoised)