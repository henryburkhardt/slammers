"""Global variables, centralizzed to one file"""

import numpy as np


GRAPH_OPTIMIZATION_EDNPOINT="http://localhost:8080/optimize"

POSE_GRAPH_FILE_PATH = "./data/graph.g2o"

ROBOT_NAME = "don"

DEFAULT_LESS_CONFIDENT_INFORMATION_MATRIX = np.array([
    [500, 0, 0],
    [0, 500, 0],
    [0, 0, 500]
])

DEFAULT_CONFIDENT_INFORMATION_MATRIX = np.array([
    [5000, 0, 0],
    [0, 5000, 0],
    [0, 0, 5000]
])


 # adjusting to account for position of lidar scanner on robot, without setting this the scans will appear off by 90 deg.
