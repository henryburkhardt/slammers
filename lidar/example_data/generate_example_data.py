import math
import numpy as np
import matplotlib.pyplot as plt
import csv


# # circle
# radius = 100    # cm 
# points = 360
# points1 = [(radius, 2 * np.pi / points * i) for i in range(points)]

# # convert circle to cartesian
# p1 = np.asarray(points1)
# r1, t1 = p1[:, 0], p1[:, 1]
# x1 = r1 * np.cos(t1)
# y1 = r1 * np.sin(t1)
# points1_cart = np.column_stack((x1, y1))

# print(points1_cart)


# square
length = 100
points = 50  # each
points1_cart = [(-1 * length + 4 * i, length) for i in range(points)]
points1_cart += [(-1 * length + 4 * i, -1 * length) for i in range(points)]
points1_cart += [(length, -1 * length + 4 * i) for i in range(points)]
points1_cart += [(-1 * length, -1 * length + 4 * i) for i in range(points)]
points1_cart = np.array(points1_cart)

# convert square to polar
p1 = np.asarray(points1_cart)
x1, y1 = p1[:, 0], p1[:, 1]
r1 = np.sqrt(np.power(x1, 2) + np.power(y1, 2))
t1 = []
for i in range(len(x1)):
    x_cur, y_cur = x1[i], y1[i]
    if x_cur >= 0:
        t1.append(np.arctan(y_cur / x_cur))
    elif y_cur > 0:
        t1.append(np.arctan(y_cur / x_cur) - np.pi)
    elif y_cur <= 0:
        t1.append(np.arctan(y_cur / x_cur) + np.pi)
t1 = np.asarray(t1)
points1 = np.column_stack((r1, t1))


# plot
x1 = points1_cart[:, 0]
y1 = points1_cart[:, 1]

plt.scatter(x1, y1, s=3, alpha=0.5, label="Target Points")

# shift points
shifted_points = points1_cart + np.array([3, 3])

# plot
x2 = shifted_points[:, 0]
y2 = shifted_points[:, 1]

plt.scatter(x2, y2, s=3, alpha=0.5, label="Original Points")

# convert to polar
p2 = np.asarray(shifted_points)
x2, y2 = p2[:, 0], p2[:, 1]
r2 = np.sqrt(np.power(x2, 2) + np.power(y2, 2))
t2 = []
for i in range(len(x2)):
    x_cur, y_cur = x2[i], y2[i]
    if x_cur >= 0:
        t2.append(np.arctan(y_cur / x_cur))
    elif y_cur > 0:
        t2.append(np.arctan(y_cur / x_cur) - np.pi)
    elif y_cur <= 0:
        t2.append(np.arctan(y_cur / x_cur) + np.pi)
t2 = np.asarray(t2)



points2 = np.column_stack((r2, t2))

plt.gca().set_aspect("equal")
plt.show()

np.savetxt('points1.csv', points1, delimiter=',')
np.savetxt('points2.csv', points2, delimiter=',')