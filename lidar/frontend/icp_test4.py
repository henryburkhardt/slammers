import cv2
import numpy as np
import matplotlib.pyplot as plt
from icp import ndt_icp2, icp
from random import sample
from utils import t2v
from math import hypot


# reading npz values and forming laserscan ndarray
def load_and_filter_scan(vertex_id):
    data = np.load(f"./data/lidar/{vertex_id}.npz")
    ranges = data["ranges"]
    angles = data["angles"]
    
    mask = np.isfinite(ranges)  # True for finite values
    ranges_filtered = ranges[mask]
    angles_filtered = angles[mask]
    
    points = np.column_stack((ranges_filtered, angles_filtered))
    return points

def filter_scan(ranges: np.ndarray, angles: np.ndarray):
    mask = np.isfinite(ranges)  # True for finite values
    ranges_filtered = ranges[mask]
    angles_filtered = angles[mask]
    
    points = np.column_stack((ranges_filtered, angles_filtered))
    return points
    

# #Create the datasets
# ang = np.linspace(-np.pi/2, np.pi/2, 320)
# pts2 = np.array([ang, np.sin(ang)])
# th = np.pi/2
# rot = np.array([[np.cos(th), -np.sin(th)],[np.sin(th), np.cos(th)]])
# pts1 = np.dot(rot, pts2) + np.array([[0.2], [0.3]])
# pts2 = pts2.T
# pts1 = pts1.T

last_vertex_points = load_and_filter_scan(vertex_id=43)
new_vertex_points = load_and_filter_scan(vertex_id=2)

larger = last_vertex_points.shape[0] < new_vertex_points.shape[0]
shape_diff = abs(new_vertex_points.shape[0] - last_vertex_points.shape[0])

# print(last_vertex_points.shape)
# print(new_vertex_points.shape)

# print(f"Larger: {larger}")
# print(f"Shape Diff: {shape_diff}")

if larger:
    indices_remove = sample(range(0, new_vertex_points.shape[0] + 1), shape_diff)
    new_vertex_points = np.delete(new_vertex_points, indices_remove, axis=0)
else:
    indices_remove = sample(range(0, last_vertex_points.shape[0] + 1), shape_diff)
    last_vertex_points = np.delete(last_vertex_points, indices_remove, axis=0)

# print(last_vertex_points.shape)
# print(new_vertex_points.shape)
# print((last_vertex_points.T)[0])


#Run the icp
# M10 = ndt_icp2(last_vertex_points, new_vertex_points, max_it=20)
M10, _, error = icp(new_vertex_points, last_vertex_points, max_iterations=20)
print(f"ERROR: {error}")
est_vec_tuple = t2v(M10)

print(f"params: {est_vec_tuple}")




distance = hypot(est_vec_tuple[0], est_vec_tuple[1])
print(f"estimated distance: {distance}")

# convert polar coordinates to cartesian coordinates (vectorized)
p1 = np.asarray(last_vertex_points)
p2 = np.asarray(new_vertex_points)

r1, t1 = p1[:, 0], p1[:, 1]
r2, t2 = p2[:, 0], p2[:, 1]

x1 = r1 * np.cos(t1)
y1 = r1 * np.sin(t1)
last_points_cart = np.column_stack((x1, y1))

x2 = r2 * np.cos(t2)
y2 = r2 * np.sin(t2)
new_points_cart = np.column_stack((x2, y2))

#Plot the result
src = np.array([new_points_cart]).astype(np.float32)
res1 = cv2.transform(src, M10)
plt.figure()
plt.plot((new_points_cart.T)[0], (new_points_cart.T)[1], marker='.', linestyle='', color='blue', markersize=1)
plt.plot(res1[0].T[0], res1[0].T[1], marker='.', linestyle='', color='red', markersize=1)
plt.plot((last_points_cart.T)[0], (last_points_cart.T)[1], marker='.', linestyle='', color='green', markersize=1)
plt.axis('equal')
plt.show()



# print(pts1)

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