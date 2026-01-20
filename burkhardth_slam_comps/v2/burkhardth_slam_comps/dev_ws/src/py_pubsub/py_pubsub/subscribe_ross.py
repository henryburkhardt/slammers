import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
import numpy as np 

class SLAMInterface(Node):

    def __init__(self):
        super().__init__('slam_interface')
        self.lidar_subscriber = self.create_subscription(
            LaserScan, 
            '/mikey/scan',  # LIDAR topic
            self.lidar_callback, 
            10
        )

        self.odom_subscriber = self.create_subscription(
            Odometry, 
            '/mikey/odom',  # Odometry topic
            self.odom_callback, 
            10
        )

    def lidar_callback(self, msg):
        scan = np.array(msg.ranges)

        print(scan)

    def odom_callback(self, msg):
        pose = (msg.pose.pose.position.x, msg.pose.pose.position.y, msg.pose.pose.orientation)


def main(args=None):
    rclpy.init(args=args)
    s = SLAMInterface()
    rclpy.spin(s)
    s.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
