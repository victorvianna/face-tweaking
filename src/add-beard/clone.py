import cv2
import numpy as numpy

source = cv2.imread("mina_size.jpg")
destination = cv2.imread("beardman.jpg")

mask = np.ones(source.shape, source.dtype)

center = (250, 250)

output = cv2.seamlessClone(source, destination, mask, center, cv2.NORMAL_CLONE)

cv2.imwrite("clone.jpg", output)