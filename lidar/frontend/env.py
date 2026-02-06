# TODO: finish env. var setup
import numpy as np

GRAPH_OPTIMIZATION_EDNPOINT="http://localhost:8080"
POSE_GRAPH_PATH = "./data/graph.g2o"

DEFAULT_CONFIDENT_INFORMATION_MATRIX = np.array([
    [1_000_000, 0, 0],
    [0, 1_000_000, 0],
    [0, 0, 1_000_000]
])

 # adjusting to account for position of lidar scanner on robot, without setting this the scans will appear off by 90 deg.
LIDAR_YAW_OFFSET = -np.pi/2 
