import os
import glob
import cv2

input_path="./dataset"
os.chdir(input_path)
images=glob.glob("*.jpg")
Length=[]
Width=[]
for img_name in images:
    print(img_name)
    img=cv2.imread(img_name)

    BLUE = [255,255,255]
    constant= cv2.copyMakeBorder(img.copy(),100,100,100,100,cv2.BORDER_CONSTANT) 
# 
    # window_name = 'image'
    # cv2.imshow(window_name, constant)  
    # cv2.waitKey(0)  
    # cv2.destroyAllWindows()  
    save_path = input_path + "/pad2" + img_name  
    print("Saving image to  : " , save_path)
    error_value = cv2.imwrite(save_path, constant)
    print(error_value)
    # if not cv2.imwrite(save_path, constant):
        # raise Exception ("Could not write Image")
