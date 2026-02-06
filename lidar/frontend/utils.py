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

def load_scans_and_filter_scan_and_also_make_them_into_points_lol(vertex_id):
    data = np.load(f"./data/lidar/{vertex_id}.npz")
    ranges = data["ranges"]
    angles = data["angles"]
    
    mask = np.isfinite(ranges)  # True for finite values
    ranges_filtered = ranges[mask]
    angles_filtered = angles[mask]
    
    points = np.column_stack((ranges_filtered, angles_filtered))
    return points
    
    

    

