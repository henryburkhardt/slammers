import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
import numpy as np
import math
from dataclasses import dataclass


class DataCollector(Node):
    def __init__(self):
        ...
        self.latest_odom = None
        self.latest_scan = None
        self.keyframes = []  # list of (pose_dict, scan_array)
        self.timer = self.create_timer(1.0, self.collect_pair)  # 1 Hz
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
import numpy as np
import math
import matplotlib.pyplot as plt
from dataclasses import dataclass

@dataclass
class Keyframe:
    pose: tuple   # (x, y, theta)
    scan: np.ndarray  # ranges (float array)
    angles: np.ndarray  # precomputed scan angles

class RealtimeMapper(Node):
    def __init__(self):
        super().__init__('realtime_mapper')

        # --- ROS Subscriptions ---
        self.create_subscription(LaserScan, '/laser_scan', self.lidar_callback, 10)
        self.create_subscription(Odometry, '/odom', self.odom_callback, 10)

        # --- Data Storage ---
        self.latest_scan = None
        self.latest_angles = None
        self.latest_odom = None
        self.keyframes = []

        # --- Timers ---
        self.create_timer(1.0, self.collect_pair)   # Grab new keyframe every second
        self.create_timer(0.5, self.update_plot)    # Refresh plot twice per second

        # --- Live Plot Setup ---
        plt.ion()
        self.fig, self.ax = plt.subplots(figsize=(6, 6))
        self.scatter_points = None
        self.robot_traj_line, = self.ax.plot([], [], 'r-', linewidth=1.5, label="Trajectory")
        self.ax.set_aspect('equal', 'box')
        self.ax.set_xlim(-10, 10)
        self.ax.set_ylim(-10, 10)
        self.ax.set_title("Real-time LiDAR Mapping")
        self.ax.set_xlabel("X [m]")
        self.ax.set_ylabel("Y [m]")
        self.ax.legend()

    # -----------------------------
    # ROS CALLBACKS
    # -----------------------------
    def odom_callback(self, msg: Odometry):
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        q = msg.pose.pose.orientation
        siny_cosp = 2 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1 - 2 * (q.y ** 2 + q.z ** 2)
        theta = math.atan2(siny_cosp, cosy_cosp)
        self.latest_odom = (x, y, theta)

    def lidar_callback(self, msg: LaserScan):
        ranges = np.array(msg.ranges, dtype=np.float32)
        angles = msg.angle_min + np.arange(len(ranges)) * msg.angle_increment
        self.latest_scan = ranges
        self.latest_angles = angles

    # -----------------------------
    # COLLECT AND STORE KEYFRAMES
    # -----------------------------
    def collect_pair(self):
        if self.latest_scan is None or self.latest_odom is None:
            self.get_logger().info("Waiting for odometry and scan...")
            return

        kf = Keyframe(
            pose=self.latest_odom,
            scan=self.latest_scan.copy(),
            angles=self.latest_angles.copy()
        )
        self.keyframes.append(kf)
        self.get_logger().info(f"Stored keyframe #{len(self.keyframes)}")

    # -----------------------------
    # REALTIME PLOTTING
    # -----------------------------
    def update_plot(self):
        if len(self.keyframes) == 0:
            return

        # Accumulate transformed points
        all_points = []
        traj_x, traj_y = [], []

        for kf in self.keyframes:
            xr, yr, th = kf.pose
            valid = np.isfinite(kf.scan)
            r, a = kf.scan[valid], kf.angles[valid]
            xs = r * np.cos(a)
            ys = r * np.sin(a)
            X = xr + xs * np.cos(th) - ys * np.sin(th)
            Y = yr + xs * np.sin(th) + ys * np.cos(th)
            all_points.append(np.column_stack((X, Y)))
            traj_x.append(xr)
            traj_y.append(yr)

        all_points = np.vstack(all_points)

        # Clear and redraw
        self.ax.clear()
        self.ax.scatter(all_points[:, 0], all_points[:, 1], s=1, c='black', alpha=0.5)
        self.ax.plot(traj_x, traj_y, 'r-', linewidth=1.5)
        self.ax.set_xlim(min(traj_x) - 5, max(traj_x) + 5)
        self.ax.set_ylim(min(traj_y) - 5, max(traj_y) + 5)
        self.ax.set_aspect('equal', 'box')
        self.ax.set_title(f"Real-time LiDAR Mapping ({len(self.keyframes)} keyframes)")
        self.ax.set_xlabel("X [m]")
        self.ax.set_ylabel("Y [m]")
        plt.draw()
        plt.pause(0.001)

def main(args=None):
    rclpy.init(args=args)
    node = RealtimeMapper()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
