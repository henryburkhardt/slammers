from ndt_icp import * 


pose1 = np.load("../data/lidar/1.npz")
pose2 = np.load("../data/lidar/2.npz")

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

# remove inf ranges

result = ndt_icp2(points1, points2)
# ndt_icp doesn't return anything yet
# ndt_icp2 returns transformaiton matrix (without bottom row)
# --> [[rot, rot, tx],
#      [rot, rot, ty]]

print(result)

# print("points 1:")
# print(points1)

# print("points 2:")
# print(points1)