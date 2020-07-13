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

## 4. &nbsp; Magic Filter 2nd attempt
- Taken from https://stackoverflow.com/questions/32913157/how-to-get-magic-color-effect-like-cam-scanner-using-opencv#62634900
- https://answers.opencv.org/question/66125/object-detection-in-nonuniform-illumination/#79578
- Was hoping for shadow and paper cringes removal.
- Didn't get satisfactory results.
- Tried 
  - Histogram equalization 
  - CLAHE
  - Tophat addition and Blackhat removal
  - Adaptive Threshold
  - fastNlMeansDenoising
- https://stackoverflow.com/questions/49997681/scanned-document-text-background-clarity-not-good-using-opencv-ios in file filter.cpp yields very promising results. There is an unimplemented part related to no text found in the image. 
- Tried Flat field correction - doesn't work so well.
- Best version till now on non-white b.g. is to simply multiply the image by 1.5 (and subtract 40, somehow this lead to bit more noise but preserves information that is otherwise lost), basically increase contrast.
  
## 5. &nbsp; bwfilter
- (Would welcome the owner to fill it)

## 6. &nbsp; filters_cpp
- https://github.com/mauvilsa/imgtxtenh 
- The procedure for compiling is:
  - ```cd build```
  - ```cmake -DCMAKE_BUILD_TYPE=Release ..```
  - ```make```
- Run with ```./imgtxtenh img_path```
- All the important functions from the purpose of the project are defined in ```filters.hpp``` and documented in ```filters.cpp```
- Good parameters for magic filter usage will be ```alpha=1.4, beta=40```

## 7. &nbsp; corners_cpp
- C++ implementation of ex02 with modifications learned during experiments.
- The procedure for compiling is: ```g++ main.cpp corners.cpp -o main.o `pkg-config --cflags --libs opencv` -DDEBUG```
- Run with ```./main.o [img_in_path] [img_out_path]``` [] is for optional arguments.
- Implements the function find_best_corners which takes in img and returns the best rectangle the algorithm managed to find.
- Implements four point transform for cropping and warping.
- Implements order points.
- Implements validate points.
- Completed with documentation.

## Remaining resources for corner detection ##
- https://stackoverflow.com/questions/6555629/algorithm-to-detect-corners-of-paper-sheet-in-photo
- https://hypjudy.github.io/2017/03/28/cvpr-A4-paper-sheet-detection-and-cropping/
- https://github.com/ctodobom/OpenNoteScanner
- https://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection
- https://stackoverflow.com/questions/49365609/get-bottomleft-topleft-corners
## Filtering and Other resources
- User manual from camscanner - http://s.intsig.net/r/manual/CamScanner_Android3.0_en-us.html
- http://www.recogniform.net/eng/how-to-enhance-scanned-documents.html
- Document Image Processing for Scanning and Printing by Ilia V. Safonov, Ilya V. Kurilin,  Michael N. Rychagov, Ekaterina V. Tolstaya. Added book in Resources. (Who's going to read this. LOL)

## OCR Resources ##
- https://tesseract-ocr.github.io/tessdoc/APIExample.html
- how-to-integrate-tesseract-ocr-library-to-a-c-program
- https://dropbox.tech/machine-learning/creating-a-modern-ocr-pipeline-using-computer-vision-and-deep-learning
- https://docparser.com/blog/improve-ocr-accuracy/
- https://stackoverflow.com/questions/23506105/extracting-text-opencv?rq=1
- https://stackoverflow.com/questions/9152803/
 
## Discussion ##
-