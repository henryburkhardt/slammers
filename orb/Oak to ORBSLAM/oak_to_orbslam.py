import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from message_filters import Subscriber, ApproximateTimeSynchronizer
from cv_bridge import CvBridge
import cv2

class OakToOrbslam(Node):
    def __init__(self):
        super().__init__('oak_to_orbslam')

        # Subscribe to rectified mono images from OAK
        self.left_sub  = Subscriber(self, Image, '/oak/left/image_rect')
        self.right_sub = Subscriber(self, Image, '/oak/right/image_rect')

        # Synchronize left/right frames
        self.ts = ApproximateTimeSynchronizer(
            [self.left_sub, self.right_sub],
            queue_size=10,
            slop=0.02
        )
        self.ts.registerCallback(self.stereo_callback)

        self.left_pub  = self.create_publisher(Image, '/orbslam/left/image_rect', 10)
        self.right_pub = self.create_publisher(Image, '/orbslam/right/image_rect', 10)

        self.bridge = CvBridge()
        self.get_logger().info('oak_to_orbslam bridge node started.')

    def stereo_callback(self, left_msg, right_msg):
        # Convert ROS to OpenCV
        left_cv  = self.bridge.imgmsg_to_cv2(left_msg, desired_encoding='passthrough')
        right_cv = self.bridge.imgmsg_to_cv2(right_msg, desired_encoding='passthrough')

        # If color, convert to grayscale
        if len(left_cv.shape) == 3:
            left_cv  = cv2.cvtColor(left_cv,  cv2.COLOR_BGR2GRAY)
            right_cv = cv2.cvtColor(right_cv, cv2.COLOR_BGR2GRAY)

        # Use OpenCV built-in FAST for quick debugging
        fast = cv2.FastFeatureDetector_create(threshold=25, nonmaxSuppression=True)
        kpts_left  = fast.detect(left_cv,  None)
        kpts_right = fast.detect(right_cv, None)

        vis_left  = cv2.drawKeypoints(left_cv,  kpts_left,  None, color=(0,255,0))
        vis_right = cv2.drawKeypoints(right_cv, kpts_right, None, color=(0,255,0))
        combined  = cv2.hconcat([vis_left, vis_right])
        cv2.imshow("Stereo features (bridge node)", combined)
        cv2.waitKey(1)

        # Convert back to ROS and publish to ORB-SLAM2 topics
        left_out  = self.bridge.cv2_to_imgmsg(left_cv,  encoding='mono8')
        right_out = self.bridge.cv2_to_imgmsg(right_cv, encoding='mono8')

        # Preserve timestamps and frame IDs
        left_out.header  = left_msg.header
        right_out.header = right_msg.header

        self.left_pub.publish(left_out)
        self.right_pub.publish(right_out)

        # Log something for your meeting demo
        self.get_logger().info(
            f"Frame {left_msg.header.stamp.sec}.{left_msg.header.stamp.nanosec}: "
            f"{len(kpts_left)} / {len(kpts_right)} keypoints"
        )

def main(args=None):
    rclpy.init(args=args)
    node = OakToOrbslam()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    node.destroy_node()
    cv2.destroyAllWindows()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
