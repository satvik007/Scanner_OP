import os
import glob
import cv2

def image_resize(image, width = None, height = None, inter = cv2.INTER_AREA):
    dim = None
    (h, w) = image.shape[:2]
    if width is None and height is None:
        return image
    if width is None:
        r = height / float(h)
        dim = (int(w * r), height)
    else:
        r = width / float(w)
        dim = (width, int(h * r))

    resized = cv2.resize(image, dim, interpolation = inter)
    return resized

input_path="./dataset"
os.chdir(input_path)
images=glob.glob("*.jpg")
Length=[]
Width=[]
for i,img_name in enumerate(images):
    print(img_name)
    img=cv2.imread(img_name)

    BLUE = [255,255,255]
    constant = cv2.copyMakeBorder(img.copy(),100,100,100,100,cv2.BORDER_REPLICATE) 
    constant = image_resize (constant , height = 800)

    save_path = str(i) + ".jpg"  
    print("Saving image to  : " , save_path)
    error_value = cv2.imwrite(save_path, constant)
    print(error_value)
    # if not cv2.imwrite(save_path, constant):
        # raise Exception ("Could not write Image")
