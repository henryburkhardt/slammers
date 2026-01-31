#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>

#include <chrono>

using namespace std::chrono_literals;

// : is for superclasses, making the superclass public allows it to be seen outside of within the instance itself. It is necessary for many ROS functions
class Publisher : public rclcpp::Node {
    std::shared_ptr<rclcpp::Publisher<std_msgs::msg::Int32>> publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    public:
    int count = 0;
    // call super constructer using colon as well. Creates node with name /orb_slam/test_listener
    Publisher() : rclcpp::Node("test_publisher", "orb_slam") {
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("test_publisher", 5);

        auto timerCallback = [this]() {
            std_msgs::msg::Int32 message;
            message.data = count;
            publisher_->publish(message);
            RCLCPP_INFO(this->get_logger(), "Sent message %i", count++);
        };

        timer_ = this->create_wall_timer(1s, timerCallback);
        
    }
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto test = std::make_shared<Publisher>();
    rclcpp::spin(test);

    return 0;
}