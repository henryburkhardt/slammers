#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

// : is for superclasses, making the superclass public allows it to be seen outside of within the instance itself. It is necessary for many ROS functions
class Listener : public rclcpp::Node {
    std::shared_ptr<rclcpp::Subscription<sensor_msgs::msg::Image>> subscription_;
    public:
    // call super constructer using colon as well. Creates node with name /orb_slam/test_listener
    Listener() : rclcpp::Node("test_listener", "orb_slam") {
        auto listenerCallback = [this](sensor_msgs::msg::Image::SharedPtr msg) {
            RCLCPP_INFO(this->get_logger(), "Received message");
        };
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>("/oakd/rgb/preview/image_raw", 5, listenerCallback);
    }
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto test = std::make_shared<Listener>();
    std::cout << "Node made." << std::endl;
    rclcpp::spin(test);

    return 0;
}