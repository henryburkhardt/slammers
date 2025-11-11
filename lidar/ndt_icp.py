import numpy as np
import math
type LaserScan = np.ndarray  # [(p1, p2), ...] OR [(r, theta), ...]


MIN_PT_CNT = 3
CELL_SIZE = 1.0   # cm
RANGE_MIN = 0.0
RANGE_MAX = 500.0

COORD_LIMIT = math.ceil(RANGE_MAX)
GRID_CNT = math.ceil(COORD_LIMIT / CELL_SIZE) * 2


def cart2idx(point: np.ndarray):
    """Converts cartesian coordinates into row, col indices for grid indexing"""
    return int(-1 * point[1] / CELL_SIZE + (GRID_CNT / 2)), int(point[0] / CELL_SIZE + (GRID_CNT / 2))


def normal_prob_cell(mean: np.ndarray, cov: np.ndarray, x: np.ndarray):
    """Returns the probability of point x being a cell sample based on given normal distribution"""
    term = x - mean
    numer = np.dot(np.dot(term, np.linalg.inv(cov)), term[:, np.newaxis])
    return np.exp(-1 * numer / 2)


def transform(point: np.ndarray, phi: float, tx: float, ty: float):
    """Returns the transformed point based on transformation parameters phi, tx, ty.
    Note that @param phi is clockwise angle."""
    matrix_rot = np.array([
        [np.cos(phi), -np.sin(phi)], [np.sin(phi), np.cos(phi)]
    ])
    return np.dot(matrix_rot, point[:, np.newaxis]).flatten() + np.array([tx, ty])


def ndt(
        points1: LaserScan, 
        points2: LaserScan, 
        phi_est: float = 0,     # clockwise
        tx_est: float = 0,
        ty_est: float = 0,
        max_it: int=10
    ):
    """Returns transformation parameter estimates (phi, t_x, t_y) from points2 to points1"""
    # convert polar coordinates to cartesian coordinates (vectorized)
    p1 = np.asarray(points1)
    p2 = np.asarray(points2)

    r1, t1 = p1[:, 0], p1[:, 1]
    r2, t2 = p2[:, 0], p2[:, 1]

    x1 = r1 * np.cos(t1)
    y1 = r1 * np.sin(t1)
    points1_cart = np.column_stack((x1, y1))

    x2 = r2 * np.cos(t2)
    y2 = r2 * np.sin(t2)
    points2_cart = np.column_stack((x2, y2))

    # split 2D space around robot into CELL_SIZE^2 cells
    grid1 = np.array([[[] for i in range(GRID_CNT)] for j in range(GRID_CNT)])          # grid1[row, col] = [point1, point2, ...]
    occu1 = np.array([[0 for i in range(GRID_CNT)] for j in range(GRID_CNT)])           # occu1[row, col] = # of points in cell
    g1ndt = np.array([[None for i in range(GRID_CNT)] for j in range(GRID_CNT)])        # g1ndt[row, col] = [mean, cov] or None

    for point in points1_cart:
        idx = cart2idx(point)
        # count grid occupancies
        occu1[idx] +=1
        # add point to cell list
        np.append(grid1[idx], point)
    
    # note which cells to compute normal distribution
    grid1_check = occu1 >= MIN_PT_CNT

    # compute first NDT
    for row in range(GRID_CNT):
        for col in range(GRID_CNT):
            # if cell has less than MIN_PT_CNT points, skip
            if not grid1_check[row, col]:
                continue
            # compute mean & covariance of cell points
            pt_mean = grid1[row, col].mean(axis=0)
            pt_cov = np.cov(grid1[row, col], rowvar=False)
            # save cell NDT
            g1ndt[row, col] = np.array([pt_mean, pt_cov])
    
    # initialize transformation parameters
    phi_cur, tx_cur, ty_cur = phi_est, tx_est, ty_est

    # optimization loop
    for it in range(max_it):
        # calculate transformation score
        total_score = 0
        for point in points2_cart:
            # map points in points2 according to transformation parameters
            new_point = transform(point, phi=phi_est, tx=tx_est, ty=ty_est)
            # find mean & covariance of corresponding cell
            idx = cart2idx(new_point)
            [pt_mean, pt_cov] = g1ndt[idx]
            # calculate point score
            pt_score = normal_prob_cell(pt_mean, pt_cov, new_point)
            total_score += pt_score
        
        # TODO: optimize with newton




if __name__ == "__main__":
    pass