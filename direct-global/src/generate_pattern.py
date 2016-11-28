import numpy as np
import cv2

h=768
w=1024
sqsize=8
step = 3
black=(0,0,0)
white=(255,255,255)
directory = "./pattern/"

def generate(offset_x,offset_y):
	img = np.zeros((h,w,3), dtype=np.uint8)
	c = np.fromfunction(lambda x,y: (((x+offset_x)//sqsize) + ((y+offset_y)//sqsize)) % 2, (h,w))
	img[c==0] = black
	img[c==1] = white
	return img


img = np.zeros((h,w,3), dtype=np.uint8)

# total black image
img[:,:] = black
cv2.imwrite(directory + 'black.jpg', img)

img[:,:] = white
cv2.imwrite(directory + 'white.jpg', img)

fnum = 1
for x in xrange(0,2*sqsize-1,step):
	for y in xrange(0,2*sqsize-1,step):
		img = generate(x, y)
		cv2.imwrite(directory + str(fnum)+'.jpg', img)
		fnum += 1

