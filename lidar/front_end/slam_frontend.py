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
from graph import Graph, Vertex, Pose2D, Edge


class SlamFrontEnd(Node):
    def __init__(self):
        super().__init__('slam_frontend')

        # ROS subscriptions
        self.create_subscription(LaserScan, '/mikey/scan', self.lidar_cb, 10)
        self.create_subscription(Odometry, '/mikey/odom', self.odom_cb, 10)

        self.latest_scan = None
        self.latest_angles = None
        self.latest_odom = None

        self.graph = Graph()

        self.last_vertex_key = None

        # Keyboard thread - to handle keypresses to trigger pose generation
        self.running = True
        self.kb_thread = threading.Thread(target=self.keyboard_listener, daemon=True)
        self.kb_thread.start()

        self.get_logger().info("SLAM Frontend running. Press SPACE to create a new pose vertex.")
    
    def reset_slam_data(
            self,
        graph_path: Path = Path("./data/graph.txt"),
        scans_dir: Path = Path("./data/scans"),
    ):
        # Delete graph file
        if graph_path.exists():
            graph_path.unlink()

        # Delete all files in scans directory
        if scans_dir.exists():
            for p in scans_dir.iterdir():
                if p.is_file():
                    p.unlink()

    
    def odom_cb(self, msg: Odometry):
        """Callback to unpack/store odom data"""
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        q = msg.pose.pose.orientation

        siny_cosp = 2 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1 - 2 * (q.y**2 + q.z**2)
        theta = math.atan2(siny_cosp, cosy_cosp)

        self.latest_odom = (x, y, theta)


    def lidar_cb(self, msg: LaserScan):
        """Callback to unpack/store lidar data"""
        self.latest_scan = np.array(msg.ranges, dtype=np.float32)
        self.latest_angles = (
            msg.angle_min
            + np.arange(len(msg.ranges)) * msg.angle_increment
        )


    def add_pose_vertex(self):
        """
        Add a pose vertext to the pose graph
        """
        if self.latest_scan is None or self.latest_odom is None:
            self.get_logger().warn("No data yet")
            return  

        # add vertext to graph
        new_vertex_key = self.graph.num_vertices + 1

        new_pose = Pose2D(self.latest_odom[0], self.latest_odom[1], self.latest_odom[2])
        
        self.graph.add_vertex(key=new_vertex_key, pose=new_pose, scan=self.latest_scan.copy(), angles=self.latest_angles.copy()) 
        
        self.save_scan_to_disk(new_vertex_key, ranges=self.latest_scan.copy(), angles=self.latest_angles.copy())

        if self.last_vertex_key == None: 
            self.last_vertex_key = new_vertex_key
            return 
        
        new_edge = Edge(dx=0, dy=0, dtheta=0, information=(1,1,1))

        self.graph.add_edge(from_key=self.last_vertex_key, to_key=new_vertex_key, edge=new_edge)

        self.graph.save_g2o(filepath="./data/graph.txt")

        self.last_vertex_key = new_vertex_key

        self.add_virtual_edges(new_vertex_key)

    def save_scan_to_disk(self, vertex_id: int, ranges, angles):
        print(ranges)
        filepath = f"./data/scans/{vertex_id}.npz"
        np.savez(filepath, ranges=ranges, angles=angles)
        return
 


    def add_virtual_edges(self, vertext_key:int):
        # TODO: finish 
        # run icp between the scan in the given vertex, and other vertices that are near by, 
        # generate an edge (or multiple?) from the response
        pass


    # Keyboard listener - to create new pose on each space bar press
    def keyboard_listener(self):
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
