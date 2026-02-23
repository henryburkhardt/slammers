import numpy as np
from icp import icp
from _ import t2v


XY_THRESHOLD = 2  # 2 meters
# TH_THRESHOLD = 3.14 * 2 * 10 / 360
USE_ODOM = False


keyframe_ids = []  # time-sensitive


# ! ADD 
def get_scan_from_id(id: int):
    return None


def scan_diff_tf(matrix):
    (x, y, theta) = t2v(matrix)
    return np.sum(np.power([x, y], [2, 2])) >= np.power(XY_THRESHOLD, 2)
    # th_cross = np.abs(theta) >= TH_THRESHOLD
    # return xy_cross or th_cross


def within_keyframe_tf(matrix):
    (x, y, theta) = t2v(matrix)
    return np.sum(np.power([x, y], [2, 2])) <= np.power(XY_THRESHOLD, 2)


# ! ADD 
def get_global_theta(id):
    return 0


# ! ADD 
def add_virtual_edge(id1, id2):
    return


def loop_closure(cur_id):
    # add keyframe at start
    if len(keyframe_ids) == 0:
        keyframe_ids.append(cur_id)
        return
    
    # check if current lidar scan is sufficiently diff. from latest keyframe
    key_scan = get_scan_from_id(keyframe_ids[-1])
    cur_scan = get_scan_from_id(cur_id)

    t_mat, _, _ = icp(key_scan, cur_scan)

    if not scan_diff_tf(t_mat):
        return
    
    # current scan is sufficiently diff. from latest keyframe
    keyframe_ids.append(cur_id)

    # compare with all previous scans
    for key_id in keyframe_ids:
        # cur_scan is now current keyframe
        key_scan = get_scan_from_id(keyframe_ids[-1])

        est_mat = None
        if USE_ODOM:
            # only use theta values
            theta_est = get_global_theta(cur_id) - get_global_theta(key_id)
            cos_val = np.cos(theta_est)
            sin_val = np.sin(theta_est)
            est_mat = np.array([[cos_val, -1 * sin_val], 
                                [sin_val, cos_val]])

        # compute icp difference
        t_mat, _, _ = icp(key_scan, cur_scan, init_pose=est_mat)
        if within_keyframe_tf(t_mat):
            add_virtual_edge(cur_id, key_id)
            break
