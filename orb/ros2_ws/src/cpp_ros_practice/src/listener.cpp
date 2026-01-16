#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>

// : is for superclasses, making the superclass public allows it to be seen outside of within the instance itself. It is necessary for many ROS functions
class Listener : public rclcpp::Node {
    std::shared_ptr<rclcpp::Subscription<std_msgs::msg::Int32>> subscription_;
    public:
    // call super constructer using colon as well. Creates node with name /orb_slam/test_listener
    Listener() : rclcpp::Node("test_listener", "orb_slam") {
        auto listenerCallback = [this](std_msgs::msg::Int32::ConstSharedPtr msg) {
            RCLCPP_INFO(this->get_logger(), "Received message %i", msg->data);
        };
        subscription_ = this->create_subscription<std_msgs::msg::Int32>("test_publisher", 5, listenerCallback);
    }
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto test = std::make_shared<Listener>();
    rclcpp::spin(test);

    return 0;
}