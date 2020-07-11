import os
import glob
import cv2

MORPH = 9
img = cv2.imread ("./dataset_bmp/blur.jpg")
kernel = cv2.getStructuringElement(cv2.MORPH_RECT,(MORPH,MORPH))
dilated = cv2.dilate(img, kernel)

cv2.imwrite ("./dataset/final.jpg" , dilated)