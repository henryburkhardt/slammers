import typing
import numpy as np
import scipy
import math
import matplotlib.pyplot as plt

LaserScan = np.ndarray  # [(p1, p2), ...] OR [(r, theta), ...]

# COPY OF ICP ALG from RAYMONDS BRANCH (just copied over). Current as of 12:05pm 1/27

MIN_PT_CNT = 3
CELL_SIZE = 50.0   # cm
RANGE_MIN = 0.0
RANGE_MAX = 500.0
IT_MAX = 100
CONV_CRITERION = 0.00000001

COORD_LIMIT = math.ceil(RANGE_MAX)
GRID_CNT = math.ceil(COORD_LIMIT / CELL_SIZE) * 2

CHECK = True
DO_PRINT = True


def cart2idx(point: np.ndarray):
    """Converts cartesian coordinates into row, col indices for grid indexing"""
    return int(-1 * point[1] / CELL_SIZE + (GRID_CNT / 2)), int(point[0] / CELL_SIZE + (GRID_CNT / 2))


################## NDT ##################


def regularize_cov(cov_matrix: np.ndarray):
    [eig_val, eig_vec] = scipy.linalg.eig(cov_matrix, check_finite=False)
    if eig_val[0] < eig_val[1]:
        eig_val[0] = eig_val[1] * 0.001
    else:
        eig_val[1] = eig_val[0] * 0.001
    diag = np.array([[eig_val[0], 0], [0, eig_val[1]]])
    reg_cov = np.dot(np.dot(eig_vec, diag), eig_vec.T)
    reg_cov = reg_cov.astype(float)
    return reg_cov


def covariance(points: list, mean: np.ndarray): 
    cov = np.array([[0.0, 0.0], [0.0, 0.0]])
    for pt in points:
        cov += np.dot((np.array(pt) - mean)[:, np.newaxis], (np.array([np.array(pt) - mean])))
    cov_matrix = cov / len(points)

    # check min eigenvalue >= 0.001 * max_eigenvalue
    eig_vals = np.sort(scipy.linalg.eigvals(cov_matrix, check_finite=False))
    if eig_vals[0] >= 0.001 * eig_vals[1]:
        return cov_matrix
    return regularize_cov(cov_matrix)


def transform(point: np.ndarray, tx: float, ty: float, phi: float):
    """Returns the transformed point based on transformation parameters phi, tx, ty.
    Note that @param phi denotes counterclockwise rotation."""
    matrix_rot = np.array([
        [np.cos(phi), -np.sin(phi)], [np.sin(phi), np.cos(phi)]
    ])
    return np.dot(matrix_rot, point[:, np.newaxis]).flatten() + np.array([tx, ty])


def coords2homo(points: np.ndarray):  # [[x1, y1], [x2, y2], ...]
    num_coords = points.shape[0]  # n * 2
    homo_coords = np.vstack([points.T, np.ones(num_coords)])
    return homo_coords  # 3 * n


def homo2coords(homo_coords: np.ndarray):
    return (homo_coords[0:2,:]).T


def transform_mat(points: np.ndarray, tx: float, ty: float, phi: float):
    cos_val = np.cos(phi)
    sin_val = np.sin(phi)
    homo_coord_mat = np.array([
        [cos_val, -sin_val, tx], 
        [sin_val, cos_val, ty], 
        [0, 0, 1],
    ])
    homo_coords = coords2homo(points)
    new_homo_coords = np.matmul(homo_coord_mat, homo_coords)
    return homo2coords(new_homo_coords)


def normal_prob_cell(mean: np.ndarray, cov: np.ndarray, x: np.ndarray):
    """Returns the probability of point x being a cell sample based on given normal distribution"""
    term = x - mean
    numer = np.dot(np.dot(term, np.linalg.inv(cov)), term[:, np.newaxis])
    return np.exp(-1 * numer / 2)


def compute_summand_increment(q: np.ndarray, xy: np.ndarray, cov: np.ndarray, cos_val: float, sin_val: float):
    """Calculates gradient/hessian components for single summand of transformation score"""
    x, y = xy
    # pre-computing values
    jacobian = np.array([[1.0, 0.0], [0.0, 1.0], [-1 * x * sin_val - y * cos_val, x * cos_val - y * sin_val]])
    cov_inv = np.linalg.inv(cov)

    q_t_cov_inv = np.dot(q, cov_inv)
    exp_term = np.exp(-1 * np.dot(q_t_cov_inv, q[:, np.newaxis]) / 2)

    # gradient
    gradient = []
    for i in range(3):
        q_partial_p_i = jacobian[i, :]
        left = np.dot(q_t_cov_inv, q_partial_p_i[:, np.newaxis])
        gradient.append(np.dot(left, exp_term))

    def q_dd(i: int, j: int):
        if i == 2 and j == 2:
            return np.array([-1 * x * cos_val + y * sin_val, -1 * x * sin_val - y * cos_val])
        return np.array([0.0, 0.0])
    
    hessian = np.array([[0.0, 0.0, 0.0] for i in range(3)])

    # hessian
    for i in range(3):
        for j in range(3):
            q_partial_p_i = jacobian[i, :]
            q_partial_p_j = jacobian[j, :]

            hess_coeff = -1 * exp_term
            term11 = np.dot(-1 * q_t_cov_inv, q_partial_p_i[:, np.newaxis])
            term12 = np.dot(-1 * q_t_cov_inv, q_partial_p_j[:, np.newaxis])
            term2 = np.dot(-1 * q_t_cov_inv, q_dd(i, j)[:, np.newaxis])
            term3 = np.dot(np.dot(-1 * q_partial_p_j, cov_inv), q_partial_p_i[:, np.newaxis])

            hessian[i, j] = np.dot(hess_coeff, np.dot(term11, term12) + term2 + term3)
    
    # # Optional: enforce symmetry (helps tiny numerical drift)
    hessian = 0.5 * (hessian + hessian.T)
    return hessian, gradient


# THIS HESSIAN SHIFT IS FUCKED
# CHANGE LATER
# def hessian_shift(hessian: np.ndarray, lam0=1e-9, factor=10.0, max_tries=8):
#     n = hessian.shape[0]
#     lam = 0.0
#     for k in range(max_tries + 1):
#         try:
#             L = np.linalg.cholesky(hessian + lam * np.eye(n))
#             return hessian + lam * np.eye(n)
#         except np.linalg.LinAlgError:
#             lam = lam0 if lam == 0.0 else lam * factor
#     # Fallback: minimal-shift eigen method
#     w = np.linalg.eigvalsh(hessian)
#     lam_min = w[0]
#     lam = 0.0 if lam_min > 0 else (-lam_min + lam0)
#     return hessian + lam * np.eye(n)


def hessian_shift(hessian: np.ndarray, factor=1.3):
    n = hessian.shape[0]
    # eig_vals = np.sort(scipy.linalg.eigvals(hessian, check_finite=False))
    eig_vals = np.sort(scipy.linalg.eigh(hessian, eigvals_only=True, check_finite=False))  # if hessian symmetry is enforced
    if DO_PRINT:
        print("eig vals:", eig_vals)
    if eig_vals[0] >= 0:
        if DO_PRINT:
            print("WARNING: eigvals aren't negative")
        return hessian
    return hessian + factor * np.abs(eig_vals[0]) * np.eye(n)


def ndt_icp(
        points1: LaserScan, 
        points2: LaserScan, 
        tx_est: float = 0.0,
        ty_est: float = 0.0,
        phi_est: float = 0.0,     # counter-clockwise
        max_it: int = IT_MAX,
        conv_line: float = CONV_CRITERION,
    ):
    """Returns transformation parameter estimates (t_x, t_y, phi) from points2 to points1"""
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

    # np.savetxt('test.txt', points2_cart, fmt='%4.6f', delimiter=',')

    # split 2D space around robot into CELL_SIZE^2 cells
    grid1 = [[[] for i in range(GRID_CNT)] for j in range(GRID_CNT)]                    # grid1[row, col] = [point1, point2, ...]
    occu1 = np.array([[0 for i in range(GRID_CNT)] for j in range(GRID_CNT)])           # occu1[row, col] = # of points in cell
    g1ndt = [[[] for i in range(GRID_CNT)] for j in range(GRID_CNT)]                    # g1ndt[row, col] = [mean, cov] or None

    for point in points1_cart:
        idx = cart2idx(point)
        # count grid occupancies
        occu1[idx] +=1
        # add point to cell list
        grid1[idx[0]][idx[1]].append(point)


    # # DEBUG
    # if CHECK:
    #     for point in points2_cart:
    #         idx = cart2idx(point)
    #         # count grid occupancies
    #         occu1[idx] +=1
    #         # add point to cell list
    #         grid1[idx[0]][idx[1]].append(point)
    #     ax = sns.heatmap(occu1)
    #     plt.gca().set_aspect("equal")
    #     # plt.imshow(occu1, cmap='hot', interpolation='nearest')
    #     plt.show()
    #     return
    
    # note which cells to compute normal distribution
    grid1_check = occu1 >= MIN_PT_CNT

    # # DEBUG
    # if CHECK:
    #     ax = sns.heatmap(grid1_check)
    #     plt.gca().set_aspect("equal")
    #     plt.show()
    #     return

    # compute first NDT
    for row in range(GRID_CNT):
        for col in range(GRID_CNT):
            # if cell has less than MIN_PT_CNT points, skip
            if not grid1_check[row, col]:
                continue
            # compute mean & covariance of cell points
            pt_mean = np.asarray(grid1[row][col]).mean(axis=0)
            pt_cov = covariance(grid1[row][col], pt_mean)
            # save cell NDT
            g1ndt[row][col] = [pt_mean, pt_cov]
    
    # initialize transformation parameters
    params = np.array([tx_est, ty_est, phi_est], dtype=np.float64)

    # optimization loop via newton's method
    prev_total_score = 0
    do_shift = False
    prev_shift = False
    prev_params = np.array([0, 0, 0])

    it = 0
    while it < max_it:
        if DO_PRINT:
            print(f"\n################ iteration {it} ################")
        if do_shift:
            if DO_PRINT:
                print("SHIFT ITERATION")
            params = prev_params.copy()
        
        if DO_PRINT:
            print("cur params:", params)

        # initialize gradient/hessian
        gradient = np.array([0.0, 0.0, 0.0])
        hessian = np.array([[0.0, 0.0, 0.0] for i in range(3)])

        # initialize transformation score
        total_score = 0

        # precomputing trig values
        cos_val = np.cos(params[2])
        sin_val = np.sin(params[2])

        new_points_mat = transform_mat(points2_cart, tx=params[0], ty=params[1], phi=params[2])
        # ! TEST IMPLEMENTATION
        for idx in range(points2_cart.shape[0]):
            point = points2_cart[idx, :]
            new_point = new_points_mat[idx, :]

        # for point in points2_cart:
        #     # map points in points2 according to transformation parameters
        #     new_point = transform(point, tx=params[0], ty=params[1], phi=params[2])
            
            # find mean & covariance of corresponding cell
            idx = cart2idx(new_point)

            # check idx isn't out of range of coordinate grid
            if not 0 <= idx[0] < GRID_CNT or not 0 <= idx[1] < GRID_CNT:
                continue
            
            # check occupancy grid
            if len(g1ndt[idx[0]][idx[1]]) == 0:
                continue

            [pt_mean, pt_cov] = g1ndt[idx[0]][idx[1]]
            # calculate point score
            pt_score = normal_prob_cell(pt_mean, pt_cov, new_point)

            total_score += pt_score

            hessian_summand, gradient_summand = compute_summand_increment(
                q=new_point - pt_mean, 
                xy=point, 
                cov=pt_cov, 
                cos_val=cos_val, 
                sin_val=sin_val
            )
            gradient += gradient_summand
            hessian += hessian_summand
        
        if DO_PRINT:
            print('total score:', total_score)

        # check for convergence
        if not do_shift and prev_total_score - conv_line <= total_score <= prev_total_score + conv_line:
            if DO_PRINT:
                print("FINAL:")
                print('hessian:\n', hessian)
                print('gradient:', gradient)
                print('params:', params)
            return params
        
        # else continue updating params
        if DO_PRINT:
            print('hessian:\n', hessian)

        if do_shift:
            hessian = hessian_shift(hessian)
            if DO_PRINT:
                print('hessian post-shift:\n', hessian)
            do_shift = False
            prev_shift = True
        else:
            # if previous iteration was hessian shift, continue
            if prev_shift:
                prev_shift = False
            # if score is decreasing, redo iteration with hessian shift to make positive definite
            elif total_score < prev_total_score:
                do_shift = True
                continue

        # update running total_score
        prev_total_score = total_score

        if DO_PRINT:
            print('gradient:', gradient)
        
        # compute transformation parameters increment
        p_delta = np.linalg.solve(hessian, -1 * gradient)
        if DO_PRINT:
            print('p_delta:', p_delta)
        
        # increment transformation parameters
        prev_params = params.copy()
        params += p_delta
        if DO_PRINT:
            print('new params:', params)

        # increase iterator
        it += 1


################## ICP ##################


if __name__ == "__main__":
    pass