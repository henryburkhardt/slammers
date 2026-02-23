import cv2
import numpy as np
import matplotlib.pyplot as plt
from icp import ndt_icp2
from utils import load_and_filter_scan

#Create the datasets
ang = np.linspace(-np.pi/2, np.pi/2, 320)
a = np.array([ang, np.sin(ang)])
th = np.pi/2
rot = np.array([[np.cos(th), -np.sin(th)],[np.sin(th), np.cos(th)]])
b = np.dot(rot, a) + np.array([[0.2], [0.3]])
a = a.T
b = b.T

def t2v(t_matrix: np.ndarray):
    """translation to vector"""
    x = t_matrix[0, 2]
    y = t_matrix[1, 2]
    theta = np.arctan2(t_matrix[1, 0], t_matrix[0, 0])
    return(x, y, theta)

print(a)

# print(a)

pose1 = np.load("./data/lidar/1.npz")
pose2 = np.load("./data/lidar/2.npz")

ranges1 = pose1["ranges"]
ranges2= pose2["ranges"]

angles1 = pose1["angles"]
angles2 = pose2["angles"]

mask = np.isfinite(ranges1)  # True for finite values
ranges1_filtered = ranges1[mask]
angles1_filtered = angles1[mask]

mask = np.isfinite(ranges2)  # True for finite values
ranges2_filtered = ranges2[mask]
angles2_filtered = angles2[mask]

points1 = np.column_stack((ranges1_filtered, angles1_filtered))
points2 = np.column_stack((ranges2_filtered, angles2_filtered))

# print(points1)


#Run the icp
# M2 = ndt_icp2(b, a, tx_est=0.1, ty_est=0.33, phi_est=np.pi/2.2, max_it=30)

points1 = load_and_filter_scan(1)
points2 = load_and_filter_scan(6)
M2 = ndt_icp2(points2, points1, max_it=30)

print("RESULT:")
print(M2)
print(t2v(M2))

# #Plot the result
# src = np.array([a]).astype(np.float32)
# res = cv2.transform(src, M2)
# plt.figure()
# plt.plot((b.T)[0], (b.T)[1])
# plt.plot(res[0].T[0], res[0].T[1], 'r.')
# plt.plot((a.T)[0], (a.T)[1])
# plt.show()