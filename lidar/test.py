import numpy as np
from numpy import genfromtxt
from ndt_icp import ndt


points1 = genfromtxt('example_data/points1.csv', delimiter=',')
points2 = genfromtxt('example_data/points2.csv', delimiter=',')


ndt(points1, points2)




# ex_points = [[ 25.88190451, -96.59258263],[ 27.56373558, -96.12616959], [ 29.23717047, -95.6304756 ]]
# ex_mean = np.array([ 27.56093685, -96.11640927])

# cov = np.array([[0.0, 0.0], [0.0, 0.0]])
# for pt in ex_points:
#     cur_cov = np.dot((np.array(pt) - ex_mean)[:, np.newaxis], (np.array([np.array(pt) - ex_mean])))
#     cov += cur_cov
# cov /= 3
# print(cov)

# point = np.array([25.88190451, -96.59258263])

# # score
# term = point - ex_mean
# numer = np.dot(np.dot(term, np.linalg.inv(cov)), term[:, np.newaxis])
# print(numer)

# score = np.exp(-1 * numer / 2)
# print("Score:", score)


# # gradient vectorization

# def compute_summand_increment(q: np.ndarray, point2: np.ndarray, cov: np.ndarray, cos_val: float, sin_val: float):
#     """Calculates gradient/hessian components for single summand of transformation score"""
#     x, y = point2
#     # pre-computing values
#     jacobian = np.array([[1, 2], [3, 4], [5, 6]])
#     cov_inv = np.linalg.inv(cov)

#     q_t_cov_inv = np.dot(q, cov_inv)
#     exp_term = np.exp(np.dot(-1 * q_t_cov_inv, q[:, np.newaxis]) / 2)

#     # gradient
#     gradient = []
#     for i in range(3):
#         q_partial_p = jacobian[i, :]
#         left = np.dot(q_t_cov_inv, q_partial_p[:, np.newaxis])
#         gradient.append(np.dot(left, exp_term))

#     print(gradient)

#     def q_der_2(i: int, j: int):
#         if i == 2 and j == 2:
#             return np.array([-1 * x * cos_val + y * sin_val, -1 * x * sin_val - y * cos_val])
#         return np.array([0, 0])
    

#     hessian = np.array([[1, 0, 0] for i in range(3)])

#     # hessian
#     for i in range(3):
#         for j in range(3):
#             q_partial_p_i = jacobian[i, :]
#             q_partial_p_j = jacobian[j, :]

#             hess_coeff = -1 * exp_term
#             term11 = np.dot(-1 * q_t_cov_inv, q_partial_p_i[:, np.newaxis])
#             term12 = np.dot(-1 * q_t_cov_inv, q_partial_p_j[:, np.newaxis])
#             term2 = np.dot(-1 * q_t_cov_inv, q_der_2(i, j)[:, np.newaxis])
#             term3 = np.dot(np.dot(q_partial_p_j, cov_inv), q_partial_p_i[:, np.newaxis])

#             print('term11', term11)
#             print('term12', term12)
#             print('term2', term2)
#             print('term3', term3)

#             hess_val = np.dot(hess_coeff, np.dot(term11, term12) + term2 + term3)
#             print(hess_val)
#             hessian[i, j] = hess_val
#     print(hessian)

# compute_summand_increment(np.array([5, 6]), np.array([4, 5]), cov, 0.6, 0.4)

