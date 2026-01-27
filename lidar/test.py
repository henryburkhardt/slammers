import time
import numpy as np
from numpy import genfromtxt
from ndt_icp import ndt_icp, transform
import matplotlib.pyplot as plt


points1 = genfromtxt('example_data/points1.csv', delimiter=',')
points2 = genfromtxt('example_data/points2.csv', delimiter=',')

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


# params = ndt_icp(points1, points2)   # tx, ty, phi
start_time = time.time()
params = ndt_icp(points1, points2, tx_est=0.0, ty_est=0.0, phi_est=0.0)
end_time = time.time()

points_estimate = np.array([transform(pt, params[0], params[1], params[2]) for pt in points2_cart])

print("Elapsed Time:", end_time - start_time)

plt.figure(figsize=(7, 7))
plt.scatter(points1_cart[:, 0], points1_cart[:, 1], s=3, alpha=0.5, label="Target Points")
plt.scatter(points2_cart[:, 0], points2_cart[:, 1], s=3, alpha=0.5, label="Original Points")
plt.scatter(points_estimate[:, 0], points_estimate[:, 1], s=3, alpha=0.5, label="Estimated Points")

plt.xlabel("x")
plt.ylabel("y")
plt.axis("equal")          # keeps units the same on both axes
plt.grid(True, linewidth=0.5, alpha=0.5)
plt.legend()
plt.tight_layout()
plt.show()

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

