#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

//to say that the callback function takes 1 input
using std::placeholders::_1;

class SimpleSubscriber : public rclcpp::Node
{
    public:
        SimpleSubscriber() : Node("simple_subscriber")
        {
            sub_ = create_subscription<std_msgs::msg::String>("chatterC",10,std::bind(&SimpleSubscriber::messageCallBack,this,_1));
        }

        void messageCallBack(const std_msgs::msg::String &message) const
        {
            RCLCPP_INFO_STREAM(get_logger(),"Reading message - " << message.data.c_str());
        }

    private:
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;

};

int main(int args, char* argv[]){
    rclcpp::init(args,argv);
    auto node = std::make_shared<SimpleSubscriber>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}