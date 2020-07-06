import cv2, numpy as np
import sys

def apply_brightness_contrast(input_img, brightness = 0, contrast = 0):

    if brightness != 0:
        if brightness > 0:
            shadow = brightness
            highlight = 255
        else:
            shadow = 0
            highlight = 255 + brightness
        alpha_b = (highlight - shadow)/255
        gamma_b = shadow

        buf = cv2.addWeighted(input_img, alpha_b, input_img, 0, gamma_b)
    else:
        buf = input_img.copy()

    if contrast != 0:
        f = 131*(contrast + 127)/(127*(131-contrast))
        alpha_c = f
        gamma_c = 127*(1-f)

        buf = cv2.addWeighted(buf, alpha_c, buf, 0, gamma_c)

    return buf

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
    denoised = cv2.fastNlMeansDenoising(adapt_thresh, 11, 45, 9) #11, 31, 9 #30,7,25

    # contrasting
    b=0 #brightness_const
    c=64 #contrast_const
    contrast = apply_brightness_contrast(denoised, b, c)

    # ---------- ignore --------- #
    # dst.convertTo(dst, -1, 2, 0)    
    # vector<Mat> channels
    # Mat img_hist_equalized
    # cvtColor(dst, img_hist_equalized, CV_BGR2YCrCb)
    # split(img_hist_equalized,channels)
    # equalizeHist(channels[0], channels[0])
    # merge(channels,img_hist_equalized)
    # cvtColor(img_hist_equalized, img_hist_equalized, CV_YCrCb2BGR)
    # --------------------------- #

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
    cv2.imshow('contrast', contrast)
    cv2.waitKey(0)

    cv2.destroyAllWindows()

    cv2.imwrite('src/ex03/images/'+'orig'+'.jpg', orig)
    cv2.imwrite('src/ex03/images/'+'grey_img'+'.jpg', grey_img)
    cv2.imwrite('src/ex03/images/'+'global_thresh'+'.jpg', global_thresh)
    cv2.imwrite('src/ex03/images/'+'adapt_thresh'+'.jpg', adapt_thresh)
    cv2.imwrite('src/ex03/images/'+'denoised'+'.jpg', denoised)
    cv2.imwrite('src/ex03/images/'+'contrast'+'.jpg', contrast)