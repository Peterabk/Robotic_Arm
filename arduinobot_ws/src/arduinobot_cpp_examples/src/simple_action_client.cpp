#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<rclcpp_components/register_node_macro.hpp>
#include<arduinobot_msgs/action/fibonacci.hpp>
#include<memory>

using namespace std::placeholders;
using namespace std::chrono_literals;

namespace arduinobot_cpp_examples{
class SimpleActionClient : public rclcpp::Node{

    public:

        explicit SimpleActionClient(const rclcpp::NodeOptions& options) : Node("simple_action_client", options){
            action_client_ = rclcpp_action::create_client<arduinobot_msgs::action::Fibonacci>(this, "fibonacci_cpp");
            timer_ = create_wall_timer(1s,std::bind(&SimpleActionClient::TimerCallback, this));
        }

    private:
        rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SharedPtr action_client_;
        rclcpp::TimerBase::SharedPtr timer_;

        //we need this method to be executed only one time, so when the callback happens we will first destroy the timer so the function
        //won't be called again by the node and the timer
        void TimerCallback()
        {
            //the first second we will call this and then destroy the timer, then send the goal
            timer_->cancel();

            if(!action_client_->wait_for_action_server()){
                RCLCPP_ERROR(get_logger(),"Action Server Not Available After Waiting");
                rclcpp::shutdown();
            }

            auto goal_msg = arduinobot_msgs::action::Fibonacci::Goal();
            goal_msg.order = 10;

            RCLCPP_INFO_STREAM(get_logger(), "Sending Goal : "<<goal_msg.order);

            auto send_goal_options = rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SendGoalOptions();

            send_goal_options.goal_response_callback = std::bind(&SimpleActionClient::ResponseCallback,this,_1);
            send_goal_options.feedback_callback = std::bind(&SimpleActionClient::FeedbackeCallback,this,_1,_2);
            send_goal_options.result_callback = std::bind(&SimpleActionClient::ResultCallback,this,_1);

            action_client_->async_send_goal(goal_msg,send_goal_options);

        }

        void ResponseCallback(const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr& goal_handle){
            if(!goal_handle){
                RCLCPP_INFO(get_logger(), "Goal Was Rejetced!");
            }else{
                RCLCPP_INFO(get_logger(), "Goal Was Accepted!");
            }
        }

        void FeedbackeCallback(rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr, 
                                const std::shared_ptr<const arduinobot_msgs::action::Fibonacci::Feedback> feedback){

            std::stringstream ss;
            ss << "Next number in sequence received: ";

            for(auto number: feedback->partial_sequence){
                ss << number << " ";
            }

            RCLCPP_INFO(get_logger(), ss.str().c_str());
        }

        void ResultCallback(const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::WrappedResult& result){
            switch (result.code)
            {
            case rclcpp_action::ResultCode::SUCCEEDED:
                /* code */
                break;

            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_INFO(get_logger(),"Goal Was ABORTED!");
                return;
            
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_INFO(get_logger(),"Goal Was CANCELED!");
                return;

            default:
                RCLCPP_INFO(get_logger(),"Unknown Result Code !");
                return;
            }

            std::stringstream ss;
            ss << "Next number in sequence received: ";

            for(auto number: result.result->sequence){
                ss << number << " ";
            }

            RCLCPP_INFO(get_logger(), ss.str().c_str());
            rclcpp::shutdown();
        }

};
}
RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_cpp_examples::SimpleActionClient);