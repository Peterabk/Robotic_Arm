#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<arduinobot_msgs/action/fibonacci.hpp>
#include<rclcpp_components/register_node_macro.hpp>

#include<memory>
#include<thread>

using namespace std::placeholders;

namespace arduinobot_cpp_examples{

class SimpleActionServer : public rclcpp::Node{
    public:
        explicit SimpleActionServer(const rclcpp::NodeOptions& options = rclcpp::NodeOptions()) : Node("simple_action_server", options)
        {
            action_server_ = rclcpp_action::create_server<arduinobot_msgs::action::Fibonacci>(
                this,
                "fibonacci_cpp",
                std::bind(&SimpleActionServer::GoalCallback,this, _1,_2),
                std::bind(&SimpleActionServer::CancelCallback,this, _1),
                std::bind(&SimpleActionServer::AcceptedCallback,this, _1)
            );

            RCLCPP_INFO(get_logger(),"Starting the Action Server Fibonacci_cpp");
        }

    private:
        rclcpp_action::Server<arduinobot_msgs::action::Fibonacci>::SharedPtr action_server_;

        //This will automatically accept the goal once it receives it
        rclcpp_action::GoalResponse GoalCallback(const rclcpp_action::GoalUUID uuid, std::shared_ptr<const arduinobot_msgs::action::Fibonacci::Goal> goal){
            RCLCPP_INFO_STREAM(get_logger(),"Received Goal Request With Order: "<< goal->order);
            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        } 

        //This will process the goal and then send an accept response, but while we're checking if we want to accept it we already start with the execution of the action
        //So that we don't take alot of time to get back to the client.
        void AcceptedCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle){
            //Create a thread to start the execution in parallel with the acceptance
            std::thread{std::bind(&SimpleActionServer::execute ,this, _1), goal_handle}.detach();
        }

        void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle){
            RCLCPP_INFO(get_logger(),"Executing goal");
            rclcpp::Rate loop_rate(1);

            const auto goal = goal_handle->get_goal();
            auto feedback = std::make_shared<arduinobot_msgs::action::Fibonacci::Feedback>();
            auto& sequence = feedback->partial_sequence;

            sequence.push_back(0);
            sequence.push_back(1);

            auto result = std::make_shared<arduinobot_msgs::action::Fibonacci::Result>();

            for(int i=1; (i < goal->order) && rclcpp::ok(); i++)
            {
                if(goal_handle->is_canceling())
                {
                    result->sequence = sequence;
                    goal_handle->canceled(result);
                    RCLCPP_INFO(get_logger(),"GOAL Canceled");
                    return;
                }

                sequence.push_back(sequence[i] + sequence[i-1]);
                goal_handle->publish_feedback(feedback);
                RCLCPP_INFO(get_logger(),"Publishing Feedback: ");
                loop_rate.sleep();
            }
            if(rclcpp::ok()){
                result->sequence = sequence;
                goal_handle->succeed(result);
                RCLCPP_INFO(get_logger(),"GOAL SUCCEEDED");
            }
        }

        rclcpp_action::CancelResponse CancelCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::Fibonacci>> goal_handle){
            RCLCPP_INFO(get_logger(),"Received request to cancel GOAL");
            return rclcpp_action::CancelResponse::ACCEPT;
        }
};
}

RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_cpp_examples::SimpleActionServer);