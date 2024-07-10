#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <arduinobot_msgs/srv/add_two_ints.hpp>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::placeholders;

class SimpleServiceClient : public rclcpp::Node{

    public:
        SimpleServiceClient(int a, int b):Node("simple_service_client"){

            client_ = create_client<arduinobot_msgs::srv::AddTwoInts>("add_two_ints");
            auto request = std::make_shared<arduinobot_msgs::srv::AddTwoInts::Request>();
            request->a = a;
            request->b = b;

            //checking if the service or if ros are available or not
            while(!client_->wait_for_service(1s)){
                RCLCPP_ERROR(get_logger(),"Service Not Available, Waiting More Time ...");
                if(!rclcpp::ok())
                {
                    RCLCPP_ERROR(get_logger(),"Interrupted while waiting for service");
                    return;
                }
            }

            auto future_result = client_->async_send_request(request, std::bind(&SimpleServiceClient::responseCallback,this, _1));
        }

    private:

        rclcpp::Client<arduinobot_msgs::srv::AddTwoInts>::SharedPtr client_;

        void responseCallback(rclcpp::Client<arduinobot_msgs::srv::AddTwoInts>::SharedFuture future){
            if(future.valid()){
                RCLCPP_INFO_STREAM(get_logger(),"Service Response sum = " << future.get()->sum);
            }
            else{
                RCLCPP_ERROR(get_logger(),"Service Failure, no sum");
            }
        }
};

int main(int args, char* argv[]){
    rclcpp::init(args,argv);

    if(args != 3){
        RCLCPP_ERROR(rclcpp::get_logger("rclpcpp"), "Wrong number of arguments! Usage: simple_service_client A B");
        return -1;
    }

    auto node = std::make_shared<SimpleServiceClient>(atoi(argv[1]), atoi(argv[2]));
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}