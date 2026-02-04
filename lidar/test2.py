import cv2
import numpy as np
import matplotlib.pyplot as plt
from ndt_icp import ndt_icp2

#Create the datasets
ang = np.linspace(-np.pi/2, np.pi/2, 320)
a = np.array([ang, np.sin(ang)])
th = np.pi/2
rot = np.array([[np.cos(th), -np.sin(th)],[np.sin(th), np.cos(th)]])
b = np.dot(rot, a) + np.array([[0.2], [0.3]])
a = a.T
b = b.T

#Run the icp
M2 = ndt_icp2(b, a, tx_est=0.1, ty_est=0.33, phi_est=np.pi/2.2, max_it=30)

#Plot the result
src = np.array([a]).astype(np.float32)
res = cv2.transform(src, M2)
plt.figure()
plt.plot((b.T)[0], (b.T)[1])
plt.plot(res[0].T[0], res[0].T[1], 'r.')
plt.plot((a.T)[0], (a.T)[1])
plt.show()