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
    