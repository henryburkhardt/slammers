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
from graph import Graph, Vertex, Pose2D

OUTPUT_FILE = Path("slam_output.npz")

@dataclass
class Keyframe:
    pose: tuple           # (x, y, theta)
    scan: np.ndarray
    angles: np.ndarray

class SlamFrontEnd(Node):
    def __init__(self):
        super().__init__('slam_frontend')

        # ROS subscriptions
        self.create_subscription(LaserScan, '/mikey/scan', self.lidar_cb, 10)
        self.create_subscription(Odometry, '/mikey/odom', self.odom_cb, 10)

        self.latest_scan = None
        self.latest_angles = None
        self.latest_odom = None
        self.keyframes = []

        self.graph = Graph()

        self.last_vertex = None

        # Keyboard thread
        self.running = True
        self.kb_thread = threading.Thread(target=self.keyboard_listener, daemon=True)
        self.kb_thread.start()

        self.get_logger().info("SLAM Frontend running. Press SPACE to save keyframe.")

    # -----------------------------
    # Callbacks
    # -----------------------------
    def odom_cb(self, msg: Odometry):
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        q = msg.pose.pose.orientation

        siny_cosp = 2 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1 - 2 * (q.y**2 + q.z**2)
        theta = math.atan2(siny_cosp, cosy_cosp)

        self.latest_odom = (x, y, theta)

    def lidar_cb(self, msg: LaserScan):
        self.latest_scan = np.array(msg.ranges, dtype=np.float32)
        self.latest_angles = (
            msg.angle_min
            + np.arange(len(msg.ranges)) * msg.angle_increment
        )

    # -----------------------------
    # Keyframe logic
    # -----------------------------
    def add_vertex(self):
        if self.latest_scan is None or self.latest_odom is None:
            self.get_logger().warn("No data yet")
            return  

        # add vertext to graph
        new_vertex_key = self.graph.num_vertices + 1
            
        self.graph.add_vertex(key=new_vertex_key, pose=self.latest_odom, scan=self.latest_scan.copy()) 

    
    def store_keyframe(self):
        if self.latest_scan is None or self.latest_odom is None:
            self.get_logger().warn("No data yet")
            return

        kf = Keyframe(
            pose=self.latest_odom,
            scan=self.latest_scan.copy(),
            angles=self.latest_angles.copy()
        )

        self.keyframes.append(kf)
        self.write_output()
        self.get_logger().info(f"Saved keyframe #{len(self.keyframes)}")

    def write_output(self):
        poses = np.array([kf.pose for kf in self.keyframes])
        scans = np.array([kf.scan for kf in self.keyframes], dtype=object)
        angles = np.array([kf.angles for kf in self.keyframes], dtype=object)

        np.savez_compressed(
            OUTPUT_FILE,
            poses=poses,
            scans=scans,
            angles=angles
        )

    # -----------------------------
    # Keyboard listener
    # -----------------------------
    def keyboard_listener(self):
        fd = sys.stdin.fileno()
        old = termios.tcgetattr(fd)
        tty.setcbreak(fd)

        try:
            while self.running:
                if select.select([sys.stdin], [], [], 0.1)[0]:
                    if sys.stdin.read(1) == ' ':
                        self.store_keyframe()
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
