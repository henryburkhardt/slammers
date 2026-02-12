import cv2
import numpy as np
import matplotlib.pyplot as plt
from ndt_icp import ndt_icp2

#Create the datasets
ang = np.linspace(-np.pi/2, np.pi/2, 320)
pts2 = np.array([ang, np.sin(ang)])
th = np.pi/2
rot = np.array([[np.cos(th), -np.sin(th)],[np.sin(th), np.cos(th)]])
pts1 = np.dot(rot, pts2) + np.array([[0.2], [0.3]])
pts2 = pts2.T
pts1 = pts1.T

print(pts1)

# #Run the icp
# M10 = ndt_icp2(pts1, pts2, tx_est=0.1, ty_est=0.33, phi_est=np.pi/2.2, max_it=10)
# M50 = ndt_icp2(pts1, pts2, tx_est=0.1, ty_est=0.33, phi_est=np.pi/2.2, max_it=50)

# #Plot the result
# src = np.array([pts2]).astype(np.float32)
# res1 = cv2.transform(src, M10)
# res2 = cv2.transform(src, M50)
# plt.figure()
# plt.plot((pts1.T)[0], (pts1.T)[1])
# plt.plot(res1[0].T[0], res1[0].T[1], 'r.')
# plt.plot(res2[0].T[0], res2[0].T[1], 'g.')
# plt.plot((pts2.T)[0], (pts2.T)[1])
# plt.show()