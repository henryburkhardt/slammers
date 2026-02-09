from icp import ndt_icp2
from utils import load_and_filter_scan, t2v
import time


start = time.time()

pose1 = load_and_filter_scan(4)
pose2 = load_and_filter_scan(3)
t_matrix = ndt_icp2(pose1, pose2)

end = time.time()
print("TIME:", end - start)

print(t_matrix)

print(t2v(t_matrix))