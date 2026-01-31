import cv2 as cv
import random

def compute_distance(des1, des2):
    distance = 0
    for i in range(len(des1)):
        distance += bin(des1[i] ^ des2[i]).count('1')
    return distance

img1 = cv.imread('im1.png', cv.IMREAD_GRAYSCALE)
img2 = cv.imread('im2.png', cv.IMREAD_GRAYSCALE)

orb = cv.ORB_create()

kp1 = orb.detect(img1, None)
kp2 = orb.detect(img2, None)

kp1, des1 = orb.compute(img1, kp1)
kp2, des2 = orb.compute(img2, kp2)

point = random.choice(list(zip(kp1, des1)))

color_img1 = cv.cvtColor(img1, cv.COLOR_GRAY2BGR)
color_img2 = cv.cvtColor(img2, cv.COLOR_GRAY2BGR)

center = [int(point[0].pt[0]), int(point[0].pt[1])]

other_point = kp2[min(range(len(des2)), key=lambda idx : compute_distance(point[1], des2[idx]))]

other_center = [int(other_point.pt[0]) + img1.shape[1], int(other_point.pt[1])]

# print(point[0].pt)
cv.circle(color_img1, center, 15, [0, 0, 255], thickness=5)
cv.circle(color_img2, other_center, 15, [0, 0, 255], thickness=5)

dest = cv.hconcat([img1, img2])
dest = cv.cvtColor(dest, cv.COLOR_GRAY2BGR)
cv.line(dest, center, other_center, [0, 0, 255], 5)

cv.imshow("im1", dest)
cv.waitKey(0)