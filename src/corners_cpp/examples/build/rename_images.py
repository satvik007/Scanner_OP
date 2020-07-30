import os
data_dir = './src/corners_cpp/examples/build/dataset/'
img_names = os.listdir(data_dir)
img_names.sort()
total_images = len(img_names)

for i, img_name in enumerate (img_names):
    if img_name.find('.jpg'):
        print (img_name)
        name, ext = img_name.split('.')
        new_name = 'img_{:02d}.{}'.format(i + 1, ext)
        os.rename(data_dir + img_name, data_dir + new_name)
