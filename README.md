# Scanner_OP #

## 1. &nbsp; Canny example 1 ##
- Taken from https://www.pyimagesearch.com/2014/09/01/build-kick-ass-mobile-document-scanner-just-5-minutes/
- added median blur and tweaked few parameters.
- Results are simply horrible.
- Works only for 1 of the examples.
- Transform library provided also yields the most horrible results I have ever seen.
- Run it with ```python src/ex01/canny.py --image data/img_02.jpg```

## 2. &nbsp; Canny example 2 ##
- Taken from https://stackoverflow.com/a/18908023/8558040
- Much better than 1st example but doesn't work on all.
- Run it with ```python src/ex02/canny.py data/img_02.jpg```
- The updated code is in the jupyter notebook.
- Improved the original code with median filter and dilation with unit kernel.
- Tried to add code for selection the best 4 points if the contour has more and failed. Will try again with more robust geometry.
- Will study bilateral filter, could be an improvement over gaussian filter in preserving edges.
- I am postponing it and will come later after trying other examples.

## 3. &nbsp; Magic Filter
- Taken from https://stackoverflow.com/questions/32913157/how-to-get-magic-color-effect-like-cam-scanner-using-opencv
- Comparing Global V/S Adaptive thresholding
- Involves: Grey scaling, thresholding, denoising
- Run it with ```python src/ex03/thresholding.py data/img_03.png```


## Remaining resources for corner detection ##
- https://stackoverflow.com/questions/6555629/algorithm-to-detect-corners-of-paper-sheet-in-photo
- https://hypjudy.github.io/2017/03/28/cvpr-A4-paper-sheet-detection-and-cropping/
- Harris Comer Detection Based on the Multi-scale
Topological Feature 
- Image Corner Detection Using Hough Transform
- https://github.com/ctodobom/OpenNoteScanner

## Discussion ##
- Please see the resources folder for the last 2 papers.
- In general we will try to avoid implementing the papers ourselves but still should look at them for ideas.
