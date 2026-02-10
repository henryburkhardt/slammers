import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Image
from cv_bridge import CvBridge

import time
import cv2 as cv
import numpy as np

captures = []

def calibrate():
    """
    Procedure modified for ROS use from OpenCV camera calibration tutorial
    """
    width, height = 9, 6 # width, height
    criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

    objp = np.zeros((width * height, 3), np.float32)
    objp[:,:2] = np.mgrid[0:width,0:height].T.reshape(-1, 2)

    objpoints = []
    imgpoints = []

    for cap in captures:
        gray = cv.cvtColor(cap, cv.COLOR_BGR2GRAY)
        ret, corners = cv.findChessboardCorners(gray, (width, height), None) # TODO make work with ros arg

        if ret:
            objpoints.append(objp)

            corners2 = cv.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
            imgpoints.append(corners2)

            cv.drawChessboardCorners(cap, (width, height), corners2, ret)
            cv.imshow('cap', cap)
            cv.waitKey(500)
    
    ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
    print(mtx)
    print(dist)

class Capturer(Node):
    def __init__(self):
        super().__init__("camsub")
        self.declare_parameter("camera", "/webcam")
        topic = self.get_parameter("camera").get_parameter_value().string_value
        self.subscription_ = self.create_subscription(Image, topic, self.cap_photo, 5)
        self.bridge = CvBridge()
        self.count = 0

    def cap_photo(self, msg: Image):
        print('hi')
        img = self.bridge.imgmsg_to_cv2(msg)
        captures.append(img)
        time.sleep(1)
        self.count += 1
        if self.count >= 20:
            rclpy.shutdown()


def main(args=None):
    rclpy.init(args=args)
    cap = Capturer()

    rclpy.spin(cap)

    cap.destroy_node()

    calibrate()
    

if __name__ == '__main__':
    main()
