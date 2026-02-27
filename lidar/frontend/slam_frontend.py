import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
import numpy as np
from random import sample
from dataclasses import dataclass
from pathlib import Path
from graph import PoseGraph, Vertex, Pose2D, Edge
from env import *
import requests 
from utils import *
from message_filters import Subscriber, ApproximateTimeSynchronizer
from utils import load_and_filter_scan
from icp import ndt_icp2, icp
from grid_map import GridMap
from nav_msgs.msg import OccupancyGrid
from std_msgs.msg import Header
from nav_msgs.msg import MapMetaData
import time
from tf2_ros import TransformBroadcaster
from geometry_msgs.msg import TransformStamped
import math
from geometry_msgs.msg import PoseStamped

## loop closure stuff
USE_LOOP = True
LOOP_THRESHOLD = 0.1
XY_THRESHOLD = 0.5  # 2 
# TH_THRESHOLD = 3.14 * 2 * 10 / 360
KEY_IGNORE = 6
USE_ODOM_FOR_LOOP_ICP = False


USE_ICP_FOR_POSE = False
ICP_IT = 10


keyframe_ids = []  # time-sensitive



class SlamFrontEnd(Node):
    def __init__(self):
        super().__init__('slam_frontend')

        self.reset_slam_data()
        
        self.logger = self.get_logger()

        # subscribers to ros topic for lidar and odom
        self.lidar_sub = Subscriber(self, LaserScan, f'/{ROBOT_NAME}/scan')
        self.odom_sub = Subscriber(self, Odometry, f'/{ROBOT_NAME}/odom')

        # synchronizer: allows 50ms tolerance (so that odom and lidar data are synced)
        self.ts = ApproximateTimeSynchronizer([self.odom_sub, self.lidar_sub], queue_size=10, slop=0.05)
        self.ts.registerCallback(self.synced_callback)

        # store the latest information from the subscriptions
        self.latest_ranges = None
        self.latest_angles = None
        self.latest_odom: Pose2D = None

        # store most recent pose and indeitifer
        self.last_added_vertex_key = None
        self.last_added_pose = None  # (x, y, theta) of last vertex
        
        # cutoff values for adding new poses
        self.min_translation = 0.1  # meters (5 cm)
        self.min_rotation = 0.4
               

        # initialize the pose graph
        self.pose_graph = PoseGraph()
        
        # intialize occupancy grid
        self.occ_pub = self.create_publisher(OccupancyGrid, '/occupancy_grid', 1)
        self.occupancy_grid = GridMap(-2, 2, -2, 2, resolution=0.05)
        self.create_timer(1.0, self.publish_occupancy_grid)
        
        # store the initial theta, so we can get new theta measurements relaitve
        self.initial_theta = 0
        self.initial_x = 0
        self.initial_y = 0
        
        self.tf_broadcaster = TransformBroadcaster(self)
        self.pose_pub = self.create_publisher(PoseStamped, '/robot_pose', 1)
        

    
    def synced_callback(self, odom: Odometry, scan: LaserScan):
        """Store latest odometry and lidar data, synchronized"""
        
        # get x,y position
        pos = odom.pose.pose.position
        
        # get rotation angle
        q = odom.pose.pose.orientation
        siny_cosp = 2 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1 - 2 * (q.y**2 + q.z**2)
        theta = math.atan2(siny_cosp, cosy_cosp)

        # update latest odom 
        current_pose = (pos.x, pos.y, theta)
        self.latest_odom = current_pose

        # convert lidar to numpy arrays and update
        self.latest_ranges = np.array(scan.ranges, dtype=np.float32)
        self.latest_angles = (
            scan.angle_min
            + np.arange(len(scan.ranges)) * scan.angle_increment
        )

        # init first pose  
        if self.last_added_pose == None and self.last_added_vertex_key == None: 
            self.initial_theta = current_pose[2]
            self.initial_x = current_pose[0]
            self.initial_y = current_pose[1]
            print(f"updated initial theta to: {self.initial_theta}")

            assert current_pose[2] - self.initial_theta == 0
            self.add_pose_vertex(pose=Pose2D(current_pose[0],current_pose[1],current_pose[2]))

            self.last_added_vertex_key = 1
            self.last_added_pose = current_pose
            return 
        
        # add new pose if robot has traveled far enought
        if(self.check_add_new_pose()):
            self.add_pose_vertex(pose=Pose2D(current_pose[0], current_pose[1], current_pose[2]))
            self.last_added_pose = current_pose
            return 
        
        return 
    

    def publish_robot_tf(self, x, y, theta):
        t = TransformStamped()
        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = "map"        # same frame as occupancy grid
        t.child_frame_id = "base_link"   # robot frame

        t.transform.translation.x = x
        t.transform.translation.y = y
        t.transform.translation.z = 0.0

        qz = math.sin(theta / 2.0)
        qw = math.cos(theta / 2.0)

        t.transform.rotation.z = qz
        t.transform.rotation.w = qw

        self.tf_broadcaster.sendTransform(t)
    
    def publish_pose_marker(self, x, y, theta):
        msg = PoseStamped()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = "map"  # must match your occupancy grid frame

        msg.pose.position.x = x
        msg.pose.position.y = y
        msg.pose.position.z = 0.0

        # convert yaw to quaternion
        import math
        qz = math.sin(theta / 2.0)
        qw = math.cos(theta / 2.0)
        msg.pose.orientation.z = qz
        msg.pose.orientation.w = qw

        self.pose_pub.publish(msg)

        
    def check_add_new_pose(self) -> bool:
        """Check if robot has traveled threshold distance"""
        
        dx = self.latest_odom[0] - self.last_added_pose[0]
        dy = self.latest_odom[1] - self.last_added_pose[1]
        dtrans = math.hypot(dx, dy)

        dtheta = abs(self.latest_odom[2] - self.last_added_pose[2])

        if dtrans > self.min_translation or dtheta > self.min_rotation:
            return True 
        return False
        
        
    def reset_slam_data(
        self,
        graph_path: Path = Path("./data/graph.txt"),
        scans_dir: Path = Path("./data/lidar"),
    ):
        """Clear all data (in the ./data folder)"""

        print("Deleting from:", scans_dir.resolve())

        
        # Delete graph file
        if graph_path.exists():
            graph_path.unlink()

        # Delete all files in scans directory
        if scans_dir.exists():
            for p in scans_dir.iterdir():
                if p.is_file():
                    p.unlink()
        return 
    
    def publish_occupancy_grid(self):
        binary_grid = self.occupancy_grid.get_binary_grid()
        msg = OccupancyGrid()
        
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'map'
        
        msg.info = MapMetaData()
        msg.info.resolution = float(self.occupancy_grid.resolution)
        msg.info.width = int(self.occupancy_grid.width)
        msg.info.height = int(self.occupancy_grid.height)
        msg.info.origin.position.x = float(6.0)
        msg.info.origin.position.y = float(6.0)
        msg.info.origin.position.z = float(0.0)
        msg.info.origin.orientation.w = float(0.0)
      
        ros_grid = np.full(binary_grid.shape, -1, dtype=np.int8)  # unknown
        ros_grid[binary_grid == 0] = 0      # free
        ros_grid[binary_grid == 1] = 100    # occupied

        msg.data = ros_grid[::-1, :].flatten().tolist()
                

        self.occ_pub.publish(msg)
        self.get_logger().debug('Published binary occupancy grid')

    def add_pose_vertex(self, pose: Pose2D, use_icp_odom=USE_ICP_FOR_POSE):
        """Add a pose vertex to the pose graph"""     
           
        assert self.latest_ranges is not None and self.latest_odom is not None

        # create new vertex id
        new_vertex_key = self.pose_graph.num_vertices + 1
        new_pose_vertex_pointcloud = filter_scan(ranges=self.latest_ranges, angles=add_nintey_to_angles(self.latest_angles.copy(), initial_theta=self.initial_theta))

        t_matrix = None

        # create the pose object (x, y, theta) and add to graph
        pose.x -= self.initial_x
        pose.y -= self.initial_y
        # pose.theta -= self.initial_theta

        x_global, y_global, t_global = 0, 0, 0
        
        if use_icp_odom and new_vertex_key != 1:
            last_pose_vertex_pointcloud = load_and_filter_scan(vertex_id=self.last_added_vertex_key)

            # load curr and prev point clouds
            
            last_pose_theta = self.pose_graph.get_vertex(self.last_added_vertex_key).pose.theta

            # run icp on the two point clouds, get translation matrix
            # t_matrix = ndt_icp2(new_vertex_points, last_vertex_points)
            theta_odom_est = pose.theta - last_pose_theta
            init_cos_val = np.cos(theta_odom_est)
            init_sin_val = np.sin(theta_odom_est)
            initial_mat = np.array([[init_cos_val, -1 * init_sin_val, 0], 
                                    [init_sin_val, init_cos_val, 0],
                                    [0, 0, 1]])

            # initial_mat = None

            t_matrix, _, _ = icp(new_pose_vertex_pointcloud, last_pose_vertex_pointcloud, init_pose=initial_mat, max_iterations=ICP_IT)

            # t_mat is currently from robot perspective, change to global translation
            (x_icp, y_icp, t_icp) = t2v(t_matrix)
            t_global = t_icp
            print(f"icp: {x_icp}, {y_icp}, {t_icp}")

            c_adj, s_adj = np.cos(last_pose_theta), np.sin(last_pose_theta)
            x_global = c_adj * x_icp - s_adj * y_icp
            y_global = s_adj * x_icp + c_adj * y_icp

            print(f"x/y global: {x_global}, {y_global}")
            
            last_pose_vector = self.pose_graph.get_vertex(self.last_added_vertex_key).to_matrix()[0:2]
            cur_pose_vector = last_pose_vector + np.array([x_global, y_global])  # translation by global tx, ty

            # print(last_pose_vector)
            # print(global_t_vec)

            pose = Pose2D(cur_pose_vector[0], cur_pose_vector[1], (last_pose_theta + t_icp))
            self.logger.debug(f"ICP diff between {new_vertex_key} and {self.last_added_vertex_key}: x:{cur_pose_vector[0]}, y:{cur_pose_vector[1]}, theta:{t_icp}")

            
            # t_vector = t2v(t_matrix) # (x y theta)
            # # compute global translation vector by rotating it by -pose_theta
            # # cos_val = np.cos(-1 * last_pose_theta)
            # # sin_val = np.sin(-1 * last_pose_theta)
            # cos_val = np.cos(last_pose_theta)
            # sin_val = np.sin(last_pose_theta)
            # rot_matrix = np.array([[cos_val, -1 * sin_val], [sin_val, cos_val]])

            # local_t_vec = np.array(t_vector[0:2])[:, np.newaxis]  # col vector
            # global_t_vec = ((rot_matrix @ local_t_vec).T)[0]  # row vector

            # # compute new pose from previous pose (global translation, then theta)

            # last_pose_vector = self.pose_graph.get_vertex(self.last_added_vertex_key).to_matrix()[0:2]
            # cur_pose_vector = last_pose_vector - global_t_vec  # translation by global tx, ty

            # # print(last_pose_vector)
            # # print(global_t_vec)
            
            # t_theta = t2v(t_matrix)[2]

            # pose = Pose2D(cur_pose_vector[0], cur_pose_vector[1], (last_pose_theta - t_theta))
            # self.logger.debug(f"ICP diff between {new_vertex_key} and {self.last_added_vertex_key}: x:{cur_pose_vector[0]}, y:{cur_pose_vector[1]}, theta:{t_theta}")
        elif not use_icp_odom and new_vertex_key != 1:
            x_global = pose.x - self.pose_graph.get_vertex(self.last_added_vertex_key).pose.x
            y_global = pose.y - self.pose_graph.get_vertex(self.last_added_vertex_key).pose.y
            t_global = pose.theta - self.pose_graph.get_vertex(self.last_added_vertex_key).pose.theta
            pass
        self.pose_graph.add_vertex(key=new_vertex_key, pose=pose, scan=self.latest_ranges.copy(), angles=add_nintey_to_angles(self.latest_angles.copy(), initial_theta=self.initial_theta)) 

        if new_vertex_key != 1:
            old_x = self.pose_graph.get_vertex(self.last_added_vertex_key).pose.x
            old_y = self.pose_graph.get_vertex(self.last_added_vertex_key).pose.y
            old_theta = self.pose_graph.get_vertex(self.last_added_vertex_key).pose.theta

            new_x = pose.x
            new_y = pose.y 
            new_theta = pose.theta

            def compute_edge(old_x, old_y, old_theta, new_x, new_y, new_theta):
                # world-frame delta
                dx_w = new_x - old_x
                dy_w = new_y - old_y

                # rotate into OLD pose frame (inverse rotation of old pose)
                c = np.cos(old_theta)
                s = np.sin(old_theta)

                dx =  c * dx_w + s * dy_w
                dy = -s * dx_w + c * dy_w

                # relative rotation
                dtheta = new_theta - old_theta
                dtheta = (dtheta + np.pi) % (2*np.pi) - np.pi  # wrap to [-pi, pi]

                return dx, dy, dtheta
                
            dx, dy, dtheta = compute_edge(
                old_x, old_y, old_theta,
                new_x, new_y, new_theta
            )

            c_val, s_val = np.cos(t_global), np.sin(t_global)
            mat_diff = np.array([[c_val, -1 * s_val, x_global], 
                                [s_val, c_val, y_global], 
                                [0, 0, 1]])

            self.pose_graph.add_edge(v1_key=self.last_added_vertex_key, v2_key=new_vertex_key, t_matrix=mat_diff, information=DEFAULT_LESS_CONFIDENT_INFORMATION_MATRIX
                                     , vector=(dx, dy, dtheta))

        
        # update the occupancy grid
        self.occupancy_grid.update_from_scan(pose=[pose.x, pose.y, pose.theta], pointcloud=new_pose_vertex_pointcloud)
        
        # save the point cloud to disk as npz (in ./data/scans)
        self.save_scan_to_disk(new_vertex_key, ranges=self.latest_ranges.copy(), angles=add_nintey_to_angles(self.latest_angles.copy(), initial_theta=self.initial_theta))


        if self.last_added_vertex_key == None: 
            self.last_added_vertex_key = new_vertex_key
            return 
                
        # save graph to g2o output file
        # TODO: make this add line by line instead of whole graph? idk
        self.pose_graph.save_graph_to_file(filepath=POSE_GRAPH_FILE_PATH, intial_theta=self.initial_theta)

        if USE_LOOP:
            loop_detected = self.loop_closure(new_vertex_key)
            if loop_detected:
                print("Trying to Optimize")
                # if a loop closure happens, we optimize the whole graph
                # self.optimize_graph()

        self.pose_graph.save_graph_to_file(filepath=POSE_GRAPH_FILE_PATH, intial_theta=self.initial_theta)

        # self.publish_pose_marker(self.last_added_pose[0], self.last_added_pose[1], self.last_added_pose[2])

        self.last_added_vertex_key = new_vertex_key
        
        self.logger.debug(f"finish adding new pose ({new_vertex_key})")
        
        return



    def save_scan_to_disk(self, vertex_id: int, ranges, angles):
        """Save lidar scan to disk as npz"""
        filepath = f"./data/lidar/{vertex_id}.npz"

        assert type(angles) == np.ndarray

        # angles += np.pi/2

        np.savez(filepath, ranges=ranges, angles=angles)
        return
    
    def optimize_graph(self):
        """Senf graph to backend to be optimized, then update graph to be the optimized version"""
        start = time.time()

        with open(POSE_GRAPH_FILE_PATH, "r") as f:
            g2o_content = f.read()
        
        # POST to backend
        response = requests.post(GRAPH_OPTIMIZATION_EDNPOINT, data=g2o_content, headers={"Content-Type": "text/plain"})

        # Check response
        if response.status_code == 200:
            print("Optimization successful!")
            optimized_content = response.text
            # Save the optimized graph
            with open("./data/graphBACK.g2o", "w") as f:
                f.write(optimized_content)
            
            # update pose graph to reflect optimized version 
            self.pose_graph.update_from_g2o("./data/graphBACK.g2o")
            self.last_added_pose = self.pose_graph.get_vertex(self.last_added_pose)
        else:
            print(f"Optimization failed! Status: {response.status_code}")
            print(response.text)

        end = time.time()
        print(f"Optimized and re-built graph in: {end - start:.6f} seconds")

        return


    def scan_diff_tf(self, matrix):
        (x, y, theta) = t2v(matrix)
        # print(f"est dist: {np.sqrt(x ** 2 + y ** 2)}")
        return np.hypot(x, y) > XY_THRESHOLD
        # th_cross = np.abs(theta) >= TH_THRESHOLD
        # return xy_cross or th_cross


    def within_keyframe_tf(self, matrix):
        (x, y, theta) = t2v(matrix)
        print(f"Check keyframe overlap: {np.hypot(x, y)}")
        return np.hypot(x, y) <= XY_THRESHOLD


    def get_global_theta(self, id):
        return self.pose_graph.get_vertex(id).pose.theta
    

    # ! ADD
    def get_global_xy(self, id):
        pose = self.pose_graph.get_vertex(id).pose
        return (pose.x, pose.y)


    def loop_closure(self, cur_id):
        # add keyframe at start
        if len(keyframe_ids) == 0:
            keyframe_ids.append(cur_id)
            print(f"Added new keyframe: {keyframe_ids}")
            return False
        
        # check if current lidar scan is sufficiently diff. from latest keyframe
        key_scan = load_and_filter_scan(keyframe_ids[-1])
        cur_scan = load_and_filter_scan(cur_id)

        # print(f"LOOP: Comparing {cur_id} to {keyframe_ids[-1]}")

        # compute initial estimate (translation and rotation)
        cur_xy = self.get_global_xy(cur_id)
        key_xy = self.get_global_xy(keyframe_ids[-1])
        cur_dir = self.get_global_theta(cur_id)
        key_dir = self.get_global_theta(keyframe_ids[-1])

        cos_val = np.cos(-1 * cur_dir)
        sin_val = np.sin(-1 * cur_dir)

        x_diff, y_diff = key_xy[0] - cur_xy[0], key_xy[1] - cur_xy[1]
        adjusted_xy = np.array([[cos_val, -1 * sin_val], [sin_val, cos_val]]) @ np.array([[x_diff], [y_diff]])
        est_rot = key_dir - cur_dir  # theta

        c_val = np.cos(-1 * est_rot)
        s_val = np.sin(-1 * est_rot)

        est_mat = np.array([[c_val, -1 * s_val, adjusted_xy[0][0]], 
                            [s_val, c_val, adjusted_xy[1][0]],
                            [0, 0, 1]])

        # compute icp with input transformation estimate
        t_mat, _, _ = icp(key_scan, cur_scan, init_pose=None)

        if self.scan_diff_tf(t_mat):
            # current scan is sufficiently diff. from latest keyframe
            keyframe_ids.append(cur_id)

            print(f"Added new keyframe: {keyframe_ids}")

        # compare with all previous scans
        for idx, key_id in enumerate(keyframe_ids):
            if len(keyframe_ids) - idx <= KEY_IGNORE:
                break
            # cur_scan is now current keyframe
            key_scan = load_and_filter_scan(key_id)

            est_mat = None
            if USE_ODOM_FOR_LOOP_ICP:
                # only use theta values
                theta_est = self.get_global_theta(cur_id) - self.get_global_theta(key_id)
                cos_val = np.cos(theta_est)
                sin_val = np.sin(theta_est)
                est_mat = np.array([[cos_val, -1 * sin_val, 0], 
                                    [sin_val, cos_val, 0]])

            # compute icp difference
            t_mat, _, error = icp(key_scan, cur_scan, init_pose=est_mat)

            # t_mat is currently from robot perspective, change to global map
            (x_icp, y_icp, t_icp) = t2v(t_mat)

            theta_adj = self.get_global_theta(cur_id)
            c_adj, s_adj = np.cos(theta_adj), np.sin(theta_adj)
            x_global = c_adj * x_icp - s_adj * y_icp
            y_global = s_adj * x_icp + c_adj * y_icp

            mat_loop = t_mat.copy()
            mat_loop[0][2] = x_global
            mat_loop[1][2] = y_global

            print(f"Error: {error}")
            if error < LOOP_THRESHOLD and self.within_keyframe_tf(t_mat):
                self.pose_graph.add_edge(v1_key=cur_id, v2_key=key_id, t_matrix=mat_loop, information=DEFAULT_CONFIDENT_INFORMATION_MATRIX, 
                                         vector=(x_global, y_global, t_icp))
                print(f"LOOP: DETECTED between {cur_id} and {key_id} with error {error}")
                return True
        return False

    def destroy_node(self):
        self.running = False
        super().destroy_node()

def main():
    rclpy.init()
    node = SlamFrontEnd()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
