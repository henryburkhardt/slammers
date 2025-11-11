import numpy as np

def my_f():
    return 0, 1


def my_f2(a, b):
    print(a, b)


arr1 = np.array([1, 2])
arr2 = np.array([[1,0], [0, 1]])
arr3 = np.array([[1], [2]])
print(arr1)
print(arr2)
print(np.dot(arr1, arr2))
print(np.dot(arr2, arr3))
print(arr1[:, np.newaxis])