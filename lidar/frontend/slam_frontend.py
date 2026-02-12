import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
import numpy as np
import math
import threading
import sys
import termios
import tty
import select
from dataclasses import dataclass
from pathlib import Path
from graph import PoseGraph, Vertex, Pose2D, Edge
from env import *
import requests 
from utils import *
from message_filters import Subscriber, ApproximateTimeSynchronizer
from utils import load_and_filter_scan
from icp import ndt_icp2



class SlamFrontEnd(Node):
    def __init__(self):
        super().__init__('slam_frontend')
        
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
        self.min_translation = 0.03  # meters (5 cm)
        self.min_rotation = 0.1
               

        # initialize the pose graph
        self.pose_graph = PoseGraph()

    
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
            self.add_pose_vertex(pose=Pose2D(current_pose[0], current_pose[1], current_pose[2]))
            self.last_added_vertex_key = 1
            self.last_added_pose = current_pose
            return 
        
        # add new pose if robot has traveled far enought
        if(self.check_add_new_pose()):
            self.add_pose_vertex(pose=Pose2D(current_pose[0], current_pose[1], current_pose[2]))
            self.last_added_pose = current_pose
            return 
        
        return 
    
    def check_add_new_pose(self) -> bool:
        """Check if robot has traveled threshold distance"""
        
        dx = self.latest_odom[0] - self.last_added_pose[0]
        dy = self.latest_odom[1] - self.last_added_pose[1]
        dtrans = math.hypot(dx, dy)

        dtheta = abs(self.latest_odom[2] - self.last_added_pose[2])

        print(dtheta)

        # RIGHT now does not consider theta
        if dtrans > self.min_translation or dtheta > self.min_rotation:
            return True 
        
        return False
        
    
    def reset_slam_data(
        self,
        graph_path: Path = Path("./data/graph.txt"),
        scans_dir: Path = Path("./data/lidar"),
    ):
        """Clear all data (in the ./data folder)"""
        
        # Delete graph file
        if graph_path.exists():
            graph_path.unlink()

        # Delete all files in scans directory
        if scans_dir.exists():
            for p in scans_dir.iterdir():
                if p.is_file():
                    p.unlink()
        return 


    def add_pose_vertex(self, pose: Pose2D, use_icp_odom=True):
        """Add a pose vertex to the pose graph"""        
        if self.latest_ranges is None or self.latest_odom is None:
            self.logger.warn("add_pose_vertex: No data yet")
            return  

        # create new verttex id
        new_vertex_key = self.pose_graph.num_vertices + 1
        
        self.logger.info(f"init adding new pose ({new_vertex_key})")
        
        if use_icp_odom and new_vertex_key != 1:
            # if using this, drive reallll slow !
            # should skip if first pose...
            last_vertex_points = load_and_filter_scan(vertex_id=self.last_added_vertex_key)
            new_vertex_points = filter_scan(ranges=self.latest_ranges, angles=self.latest_angles)

            t_matrix = ndt_icp2(new_vertex_points, last_vertex_points)
            t_vector = t2v(t_matrix) # (x y theta)

            # t_matrix = np.linalg.inv(t_matrix)
            last_pose_theta = self.pose_graph.get_vertex(self.last_added_vertex_key).pose.theta
            
            # compute global translation vector by rotating it by -pose_theta
            cos_val = np.cos(-1 * last_pose_theta)
            sin_val = np.sin(-1 * last_pose_theta)
            # cos_val = np.cos(last_pose_theta)
            # sin_val = np.sin(last_pose_theta)
            rot_matrix = np.array([[cos_val, -1 * sin_val], [sin_val, cos_val]])

            local_t_vec = np.array(t_vector[0:2])[:, np.newaxis]  # col vector
            global_t_vec = ((rot_matrix @ local_t_vec).T)[0]  # row vector

            # compute new pose from previous pose (global translation, then theta)

            last_pose_vector = self.pose_graph.get_vertex(self.last_added_vertex_key).to_matrix()[0:2]
            cur_pose_vector = last_pose_vector + global_t_vec  # translation by global tx, ty

            print(last_pose_vector)
            print(global_t_vec)
            
            t_theta = t2v(t_matrix)[2]

            print("T THETA:", t_theta)

            pose = Pose2D(cur_pose_vector[0], cur_pose_vector[1], last_pose_theta - t_theta)
            # self.logger.info(f"ICP diff between {new_vertex_key} and {self.last_added_vertex_key}: x:{vector[0]}")

        # create the pose object (x, y, theta) and add to graph
        self.pose_graph.add_vertex(key=new_vertex_key, pose=pose, scan=self.latest_ranges.copy(), angles=self.latest_angles.copy()) 
        
        # save the point cloud to disk as npz (in ./data/scans)
        self.save_scan_to_disk(new_vertex_key, ranges=self.latest_ranges.copy(), angles=self.latest_angles.copy())

        # TODO: do we need this part below?
        if self.last_added_vertex_key == None: 
            self.last_added_vertex_key = new_vertex_key
            return 
        
        # self.pose_graph.add_edge(v1_key=self.last_added_vertex_key, v2_key=new_vertex_key, information=DEFAULT_CONFIDENT_INFORMATION_MATRIX)
        
        # save graph to g2o output file
        # TODO: make this add line by line instead of whole graph? idk
        self.pose_graph.save_graph_to_file(filepath=POSE_GRAPH_FILE_PATH)

        self.last_added_vertex_key = new_vertex_key

        self.add_virtual_edges(new_vertex_key)
        
        self.logger.info(f"finish adding new pose ({new_vertex_key})")
        
        return



    def save_scan_to_disk(self, vertex_id: int, ranges, angles):
        """Save lidar scan to disk as npz"""
        filepath = f"./data/lidar/{vertex_id}.npz"
        np.savez(filepath, ranges=ranges, angles=angles)
        return
 

    def is_pose_near_previsouly_explored_area(self, vertex_key) -> bool:
        """check if given pose is an a previsouly explored area that is being revisited"""
        # TODO: finish function
        return True

    def add_virtual_edges(self, vertext_key:int):
        # TODO: finish 
        # check if the pose is back in an area that the robot has not visited for a while
        if(self.is_pose_near_previsouly_explored_area(vertext_key)):
            # run loop closure alg!
            pass
        else: 
            return 
    
    async def optimize_graph(self):
        """Senf graph to backend to be optimized, then update graph to be the optimized version"""
        with open(POSE_GRAPH_FILE_PATH, "r") as f:
            g2o_content = f.read()
        
        # POST to backend
        response = requests.post(GRAPH_OPTIMIZATION_EDNPOINT, data=g2o_content, headers={"Content-Type": "text/plain"})

        # Check response
        if response.status_code == 200:
            print("Optimization successful!")
            optimized_content = response.text
            # Save the optimized graph
            with open(POSE_GRAPH_FILE_PATH, "w") as f:
                f.write(optimized_content)
            
            # update pose graph to reflect optimized version 
            self.pose_graph.update_from_g2o(POSE_GRAPH_FILE_PATH)
        else:
            print(f"Optimization failed! Status: {response.status_code}")
            print(response.text)
        return

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
