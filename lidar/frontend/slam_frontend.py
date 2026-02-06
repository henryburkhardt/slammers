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
from utils import load_scans_and_filter_scan_and_also_make_them_into_points_lol
from icp import ndt_icp2



class SlamFrontEnd(Node):
    def __init__(self):
        super().__init__('slam_frontend')

        # subscribers to ros topic for lidar and odom
        self.lidar_sub = Subscriber(self, LaserScan, '/mikey/scan')
        self.odom_sub = Subscriber(self, Odometry, '/mikey/odom')

        # synchronizer: allows 50ms tolerance (so that odom and lidar data are synced)
        self.ts = ApproximateTimeSynchronizer([self.odom_sub, self.lidar_sub], queue_size=10, slop=0.05)
        self.ts.registerCallback(self.synced_callback)

        # store the latest information from the subscriptions
        self.latest_ranges = None
        self.latest_angles = None
        self.latest_odom = None

        # store most recent pose and indeitifer
        self.last_vertex_key = None
        self.last_added_pose = None  # (x, y, theta) of last vertex
        
        # cutoff values for adding new poses
        self.min_translation = 0.05  # meters (5 cm)
        self.min_rotation = 0.01  
               

        # initialize the pose graph
        self.pose_graph = PoseGraph()

        # Keyboard thread - to handle keypresses to trigger pose generation
        self.running = True
        self.kb_thread = threading.Thread(target=self.keyboard_listener, daemon=True)
        self.kb_thread.start()

        self.get_logger().info("SLAM Frontend running.")
    
    def synced_callback(self, odom: Odometry, scan: LaserScan):
        """Callback receives synchronized odometry + lidar scan"""
        # convert odometry quaternion -> yaw (2D pose)
        pos = odom.pose.pose.position
        q = odom.pose.pose.orientation
        
        siny_cosp = 2 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1 - 2 * (q.y**2 + q.z**2)
        theta = math.atan2(siny_cosp, cosy_cosp)

        # update odom 
        self.latest_odom = (pos.x, pos.y, theta)

        # convert lidar to numpy arrays and update
        self.latest_ranges = np.array(scan.ranges, dtype=np.float32)
        self.latest_angles = (
            scan.angle_min
            + np.arange(len(scan.ranges)) * scan.angle_increment
        )
    
    def reset_slam_data(
        self,
        graph_path: Path = Path("./data/graph.txt"),
        scans_dir: Path = Path("./data/lidar"),
    ):
        """clears all data (in the ./data folder)"""
        # Delete graph file
        if graph_path.exists():
            graph_path.unlink()

        # Delete all files in scans directory
        if scans_dir.exists():
            for p in scans_dir.iterdir():
                if p.is_file():
                    p.unlink()

    
    # def odom_cb(self, msg: Odometry):
    #     """Callback to unpack/store odom data -- called everytime there is fresh odom data"""
    #     x = msg.pose.pose.position.x
    #     y = msg.pose.pose.position.y
    #     q = msg.pose.pose.orientation

    #     siny_cosp = 2 * (q.w * q.z + q.x * q.y)
    #     cosy_cosp = 1 - 2 * (q.y**2 + q.z**2)
    #     theta = math.atan2(siny_cosp, cosy_cosp)

    #     self.latest_odom = (x, y, theta)


    # def lidar_cb(self, msg: LaserScan):
    #     """Callback to unpack/store lidar data -- called everytime there is fresh lidar data"""
    #     self.latest_ranges = np.array(msg.ranges, dtype=np.float32)
    #     self.latest_angles = (
    #         msg.angle_min
    #         + np.arange(len(msg.ranges)) * msg.angle_increment
    #     )


    def add_pose_vertex(self, use_icp_odom=False):
        """Add a pose vertex to the pose graph"""
        if self.latest_ranges is None or self.latest_odom is None:
            self.get_logger().warn("No data yet")
            return  

        # create new verttex id
        new_vertex_key = self.pose_graph.num_vertices + 1
        
        if use_icp_odom:
            last_vertex_points = load_scans_and_filter_scan_and_also_make_them_into_points_lol(vertex_id=self.last_vertex_key)
            new_vertex_points = load_scans_and_filter_scan_and_also_make_them_into_points_lol(vertex_id=new_pose)
            M2 = ndt_icp2(last_vertex_points, new_vertex_points)

        # create the pose object (x, y, theta) and add to graph
        new_pose = Pose2D(self.latest_odom[0], self.latest_odom[1], self.latest_odom[2])
        self.pose_graph.add_vertex(key=new_vertex_key, pose=new_pose, scan=self.latest_ranges.copy(), angles=self.latest_angles.copy()) 
        
        # save the point cloud to disk as npz (in ./data/scans)
        self.save_scan_to_disk(new_vertex_key, ranges=self.latest_ranges.copy(), angles=self.latest_angles.copy())

        if self.last_vertex_key == None: 
            self.last_vertex_key = new_vertex_key
            return 
        
        self.pose_graph.add_edge(v1_key=self.last_vertex_key, v2_key=new_vertex_key, information=DEFAULT_CONFIDENT_INFORMATION_MATRIX)
        
        # save graph to g2o output file
        self.pose_graph.save_graph_to_file(filepath="./data/graph.g2o")

        self.last_vertex_key = new_vertex_key

        self.add_virtual_edges(new_vertex_key)

    def save_scan_to_disk(self, vertex_id: int, ranges, angles):
        """save lidar scan to disk as npz"""
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
        """send graph to backend to be optimized, then update graph to be the optimized version"""
        with open(POSE_GRAPH_PATH, "r") as f:
            g2o_content = f.read()
            # POST to backend
        response = requests.post(GRAPH_OPTIMIZATION_EDNPOINT, data=g2o_content, headers={"Content-Type": "text/plain"})

        # Check response
        if response.status_code == 200:
            print("Optimization successful!")
            optimized_content = response.text
            # Save the optimized graph
            with open(POSE_GRAPH_PATH, "w") as f:
                f.write(optimized_content)
            
            # update pose graph to reflect optimized version 
            self.pose_graph.update_from_g2o(POSE_GRAPH_PATH)
        else:
            print(f"Optimization failed! Status: {response.status_code}")
            print(response.text)
        return



    def keyboard_listener(self):
        """Keyboard listener - to create new pose on each space bar press"""
        self.reset_slam_data()

        fd = sys.stdin.fileno()
        old = termios.tcgetattr(fd)
        tty.setcbreak(fd)

        try:
            while self.running:
                if select.select([sys.stdin], [], [], 0.1)[0]:
                    if sys.stdin.read(1) == ' ':
                        self.add_pose_vertex()
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old)

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
