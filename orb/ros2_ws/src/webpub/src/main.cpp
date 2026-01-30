#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/header.hpp>
#include <rclcpp/rclcpp.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>

class WebcamPublisher : public rclcpp::Node {
    public:
    WebcamPublisher() : rclcpp::Node("webcam_publisher") {
        auto timer_callback = [this]{
            cv::Mat frame;
            bool ret = webcam.read(frame);
            if (ret) {
                std_msgs::msg::Header header;
                header.stamp = this->get_clock()->now();
                cv_bridge::CvImage img(header, "bgr8", frame);
                this->publisher_->publish(*img.toImageMsg());
            }
        };
        webcam = cv::VideoCapture("IMG_0449.MOV");
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/webcam", 5);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(33), timer_callback);
    }

    private:
    std::shared_ptr<rclcpp::Publisher<sensor_msgs::msg::Image>> publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::VideoCapture webcam;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    std::shared_ptr<WebcamPublisher> webpub = std::make_shared<WebcamPublisher>();
    rclcpp::spin(webpub);
    rclcpp::shutdown();
}