
import numpy as np
import cv2
import sys
import os

if (len(sys.argv) != 2):
	print "Usage : python main.py <path_to_image_directory>\n\n"
	exit(0)

file_path = sys.argv[1]
file_path += "/"
files = os.listdir(file_path)

# check there are 25 images
if len(files) != 25:
	print "There should be only 25 images in the directory. Exitting!\n\n"
	exit(0)

img = cv2.imread(file_path + files[0])
max_img = img
min_img = img
for f in files:
	img = cv2.imread(file_path + f)
	max_img = np.maximum(max_img, img)
	min_img = np.minimum(min_img, img)


b=0.01
direct_img = (max_img - min_img)/(1.0-b)
direct_img = np.minimum(direct_img,255)
global_img = (min_img - b*max_img)*(2.0/(1.0-b*b))
global_img = np.minimum(global_img, 255)

cv2.imwrite(file_path + "direct.jpg", direct_img)
cv2.imwrite(file_path + "global.jpg", global_img)

