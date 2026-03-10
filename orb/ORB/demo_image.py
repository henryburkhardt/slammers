import numpy as np
import cv2 as cv
from random import randint


def random_image():
    img = np.zeros(dtype=np.uint8, shape=(500, 500))

    for i in range(5):
        for j in range(5):
            cv.rectangle(img, (100 * i, 100 * j), (100 * i + 100, 100 * j + 100), randint(0, 255), -1)

    cv.imwrite("demo_img.png", img)


def rectangle():
    img = np.zeros(dtype=np.uint8, shape=(480, 640))
    cv.rectangle(img, (100, 100), (540, 380), color=255, thickness=-1)
    cv.imwrite("demo_img.png", img)


def radial_gradient(radius: int):
    img = np.zeros(dtype=np.uint8, shape=(501, 501))
    for i in range(501):
        for j in range(501):
            img[i, j] = round(min(((i - 250) ** 2 + (j - 250) ** 2) / (radius ** 2), 1) * 255)

    # cv.rectangle(img, (250, 249), (500, 251), 255, -1)
    cv.imwrite("demo_img.png", img)
        
radial_gradient(250)