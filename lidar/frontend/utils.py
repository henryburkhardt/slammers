import numpy as np 

def se2_relative_transformation(m1:np.ndarray, m2:np.ndarray):
    """relative transformation FROM m1 TO m2"""
    return np.linalg.inv(m1) @ m2
    
def t2v(t_matrix: np.ndarray):
    """translation to vector"""
    x = t_matrix[0, 2]
    y = t_matrix[1, 2]
    theta = np.arctan2(t_matrix[1, 0], t_matrix[0, 0])
    return(x, y, theta)

def load_and_filter_scan(vertex_id):
    data = np.load(f"./data/lidar/{vertex_id}.npz")
    ranges = data["ranges"]
    angles = data["angles"]
    
    mask = np.isfinite(ranges) & (ranges >= 0.2)  # True for finite values
    ranges_filtered = ranges[mask]
    angles_filtered = angles[mask]
    
    points = np.column_stack((ranges_filtered, angles_filtered))
    return points

def filter_scan(ranges: np.ndarray, angles: np.ndarray):
    mask = np.isfinite(ranges) & (ranges >= 0.2) # True for finite values
    ranges_filtered = ranges[mask]
    angles_filtered = angles[mask]
    
    points = np.column_stack((ranges_filtered, angles_filtered))
    return points

def load_scan(vertex_id):
    data = np.load(f"./data/lidar/{vertex_id}.npz")
    ranges = data["ranges"]
    angles = data["angles"]

    points = np.column_stack((ranges, angles))
    return points

def add_nintey_to_angles(angles, initial_theta):
    assert type(angles) == np.ndarray

    # angles += np.pi/2 + initial_theta
    angles += np.pi/2

    return angles