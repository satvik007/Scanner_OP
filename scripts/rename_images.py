import os
data_dir = '../data/'
img_names = os.listdir(data_dir)
total_images = len(img_names)

for i, img_name in enumerate (img_names):
    name, ext = img_name.split('.')
    new_name = 'img_{:02d}.{}'.format(i + 1, ext)
    os.rename(data_dir + img_name, data_dir + new_name)
