#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <chrono>

using namespace std::chrono_literals;

//inhereting the Node class
class SimplePublisher : public rclcpp::Node{

public:
    SimplePublisher() : Node("simple_publisher"), counter_{0}
    {
        //Create the publisher with the name and number of message queue
        pub_ = create_publisher<std_msgs::msg::String>("chatterC",10);

        //binding the call back with the class instance,
        // and showing that the callback function is from the class
        timer_ = create_wall_timer(1s,std::bind(&SimplePublisher::timerCallBack,this));

        //Logger to output messages in kernel
        RCLCPP_INFO(get_logger(), "Publishing at 1 HZ");
    }

    void timerCallBack()
    {
        //creating a message variable to send
        auto message = std_msgs::msg::String();
        message.data = "Hello Ros2 - counter " + std::to_string(counter_++);
        pub_->publish(message);
    }

private:
    unsigned int counter_;
    //sharedptr is the pointer used to refer to the publisher and timer
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int args, char* argv[])
{
    rclcpp::init(args,argv);
    auto node = std::make_shared<SimplePublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0; 
}