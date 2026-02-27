import typing
import numpy as np
import scipy
import math
# import seaborn as sns
import matplotlib.pyplot as plt
from random import sample


LaserScan = np.ndarray  # [(p1, p2), ...] OR [(r, theta), ...]


MIN_PT_CNT = 3
CELL_SIZE = 50.0   # cm
RANGE_MIN = 0.0
RANGE_MAX = 500.0
IT_MAX = 100
CONV_CRITERION = 0.00000001

COORD_LIMIT = math.ceil(RANGE_MAX)
GRID_CNT = math.ceil(COORD_LIMIT / CELL_SIZE) * 2

CHECK = True
DO_PRINT = False

MEASURE_RATIO = 0.855
LIDAR_SHIFT = 0.05 * MEASURE_RATIO  # x-axis direction


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


# QUICK NDP_ICP

import cv2
import numpy as np
import matplotlib.pyplot as plt
from sklearn.neighbors import NearestNeighbors


def ndt_icp2(
        points1: LaserScan, 
        points2: LaserScan, 
        tx_est: float = 0.0,
        ty_est: float = 0.0,
        phi_est: float = 0.0,     # counter-clockwise
        max_it: int = IT_MAX,
    ):

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

    source = np.array([points2_cart], copy=True).astype(np.float32)
    dest = np.array([points1_cart], copy=True).astype(np.float32)

    # source = np.array([points2], copy=True).astype(np.float32)
    # dest = np.array([points1], copy=True).astype(np.float32)

    #Initialise with the initial pose estimation
    transform_matrix = np.array([
        [np.cos(phi_est), -np.sin(phi_est), tx_est],
        [np.sin(phi_est), np.cos(phi_est), ty_est],
        [0, 0, 1]]
    )

    # print("starting source:", source)
    # source = cv2.transform(source, transform_matrix[0:2])
    # source = cv2.warpAffine(source, transform_matrix[0:2], (source.shape[1], source.shape[0]))
    source = cv2.transform(source, transform_matrix[0:2])
    print(dest)

    for i in range(max_it):
        # print("source shape:", source.shape)
        # print("current source:", source)
        # print("dest:", dest[0])
        neighbors = NearestNeighbors(n_neighbors=1, algorithm='auto',).fit(dest[0])
        # print("neighbors:", neighbors)
        distances, indices = neighbors.kneighbors(source[0])
        # print("indices:", indices)
        # print("dest input:", dest[0, indices.T])
        T = cv2.estimateAffinePartial2D(source, dest[0, indices.T])
        # print("estimated:", T[0])
        source = cv2.transform(source, T[0])
        # print("source shape:", source.shape)
        transform_matrix = np.dot(transform_matrix, np.vstack((T[0],[0,0,1])))
        # print(i, f"\n{transform_matrix}")
        # print("trans_matrix:", transform_matrix)
    return transform_matrix[0:2]

import numpy as np
from sklearn.neighbors import NearestNeighbors


def best_fit_transform(A, B):
    '''
    Calculates the least-squares best-fit transform that maps corresponding points A to B in m spatial dimensions
    Input:
      A: Nxm numpy array of corresponding points
      B: Nxm numpy array of corresponding points
    Returns:
      T: (m+1)x(m+1) homogeneous transformation matrix that maps A on to B
      R: mxm rotation matrix
      t: mx1 translation vector
    '''

    assert A.shape == B.shape

    # get number of dimensions
    m = A.shape[1]

    # translate points to their centroids
    centroid_A = np.mean(A, axis=0)
    centroid_B = np.mean(B, axis=0)
    AA = A - centroid_A
    BB = B - centroid_B

    # rotation matrix
    H = np.dot(AA.T, BB)
    U, S, Vt = np.linalg.svd(H)
    R = np.dot(Vt.T, U.T)

    # special reflection case
    if np.linalg.det(R) < 0:
       Vt[m-1,:] *= -1
       R = np.dot(Vt.T, U.T)

    # translation
    t = centroid_B.T - np.dot(R,centroid_A.T)

    # homogeneous transformation
    T = np.identity(m+1)
    T[:m, :m] = R
    T[:m, m] = t

    return T, R, t


def nearest_neighbor(src, dst):
    '''
    Find the nearest (Euclidean) neighbor in dst for each point in src
    Input:
        src: Nxm array of points
        dst: Nxm array of points
    Output:
        distances: Euclidean distances of the nearest neighbor
        indices: dst indices of the nearest neighbor
    '''

    assert src.shape == dst.shape

    neigh = NearestNeighbors(n_neighbors=1)
    neigh.fit(dst)
    distances, indices = neigh.kneighbors(src, return_distance=True)
    return distances.ravel(), indices.ravel()


def icp(A, B, init_pose=None, max_iterations=20, tolerance=0.001):
    '''
    The Iterative Closest Point method: finds best-fit transform that maps points A on to points B
    Input:
        A: Nxm numpy array of source mD points
        B: Nxm numpy array of destination mD point
        init_pose: (m+1)x(m+1) homogeneous transformation
        max_iterations: exit algorithm after max_iterations
        tolerance: convergence criteria
    Output:
        T: final homogeneous transformation that maps A on to B
        distances: Euclidean distances (errors) of the nearest neighbor
        i: number of iterations to converge
    '''
    
    larger = A.shape[0] < B.shape[0]
    shape_diff = abs(B.shape[0] - A.shape[0])

    # print(last_pose_vertext_pointcloud.shape)
    # print(new_pose_vertex_pointcloud.shape)

    # print(f"Larger: {larger}")
    # print(f"Shape Diff: {shape_diff}")

    # make the point clouds the same size, by radomly removing points from one. TEMPORARY FIX.
    if larger:
        indices_remove = sample(range(0, B.shape[0]), shape_diff)
        B = np.delete(B, indices_remove, axis=0)
    else:
        indices_remove = sample(range(0, A.shape[0]), shape_diff)
        A = np.delete(A, indices_remove, axis=0)

    assert A.shape == B.shape

    # convert polar coordinates to cartesian coordinates (vectorized)
    p1 = np.asarray(A)
    p2 = np.asarray(B)

    r1, t1 = p1[:, 0], p1[:, 1]
    r2, t2 = p2[:, 0], p2[:, 1]

    x1 = r1 * np.cos(t1)
    y1 = r1 * np.sin(t1)
    points1_cart = np.column_stack((x1, y1))

    x2 = r2 * np.cos(t2)
    y2 = r2 * np.sin(t2)
    points2_cart = np.column_stack((x2, y2))

    # shift points by x-value to account for lidar sensor location
    points1_cart[:, 0] -= LIDAR_SHIFT
    points2_cart[:, 0] -= LIDAR_SHIFT

    # get number of dimensions
    m = A.shape[1]

    # make points homogeneous, copy them to maintain the originals
    src = np.ones((m+1,points1_cart.shape[0]))
    dst = np.ones((m+1,points2_cart.shape[0]))
    src[:m,:] = np.copy(points1_cart.T)
    dst[:m,:] = np.copy(points2_cart.T)

    # apply the initial pose estimation
    if init_pose is not None:
        src = np.dot(init_pose, src)

    # # min mean distances isn't necessarly small even if estimation is good (bc different # of points)
    # min_mean_distances = 0

    close_perc = 0
    prev_error = 0

    for i in range(max_iterations):
        # find the nearest neighbors between the current source and destination points
        distances, indices = nearest_neighbor(src[:m,:].T, dst[:m,:].T)

        # compute the transformation between the current source and nearest destination points
        T,_,_ = best_fit_transform(src[:m,:].T, dst[:m,indices].T)

        # update the current source
        src = np.dot(T, src)

        # check error
        mean_error = np.mean(distances)
        # min_mean_distances = mean_error
        close_tf = distances <= 0.1
        close_perc = sum(close_tf) / len(close_tf)

        if np.abs(prev_error - mean_error) < tolerance:
            break
        prev_error = mean_error
    

    # calculate final transformation
    T,_,_ = best_fit_transform(points1_cart, src[:m,:].T)

    # return T, distances, i
    return T, distances, 1 - close_perc


if __name__ == "__main__":
    pass