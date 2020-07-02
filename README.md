# Scanner_OP #

## 1. &nbsp; Canny example 1 ##
- Taken from https://www.pyimagesearch.com/2014/09/01/build-kick-ass-mobile-document-scanner-just-5-minutes/
- added median blur and tweaked few parameters.
- Results are simply horrible.
- Works only for **data/ds5.jpg**.
- Transform library provided also yields the most horrible results I have ever seen.
- Run it with ```python src/ex01/canny.py --image data/ds5.jpg```

## 2. &nbsp; Canny example 2 ##
- Taken from https://stackoverflow.com/a/18908023/8558040
- Much better than 1st example but doesn't work on all.
- Run it with ```python src/ex02/canny.py data/ds5.jpg```
- Still working on it.

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
