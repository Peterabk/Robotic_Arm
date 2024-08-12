#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<arduinobot_msgs/action/arduinobot_task.hpp>
#include<rclcpp_components/register_node_macro.hpp>
#include<moveit/move_group_interface/move_group_interface.h>

#include<memory>
#include<thread>

using namespace std::placeholders;

namespace arduinobot_remote{

class TaskServer : public rclcpp::Node{
    public:
        explicit TaskServer(const rclcpp::NodeOptions& options = rclcpp::NodeOptions()) : Node("task_server", options)
        {
            action_server_ = rclcpp_action::create_server<arduinobot_msgs::action::ArduinobotTask>(
                this,
                "task_server",
                std::bind(&TaskServer::GoalCallback,this, _1,_2),
                std::bind(&TaskServer::CancelCallback,this, _1),
                std::bind(&TaskServer::AcceptedCallback,this, _1)
            );

            RCLCPP_INFO(get_logger(),"Starting the Action Server : Task_server");
        }

    private:
        rclcpp_action::Server<arduinobot_msgs::action::ArduinobotTask>::SharedPtr action_server_;

        //This will automatically accept the goal once it receives it
        rclcpp_action::GoalResponse GoalCallback(const rclcpp_action::GoalUUID uuid, std::shared_ptr<const arduinobot_msgs::action::ArduinobotTask::Goal> goal){
            RCLCPP_INFO_STREAM(get_logger(),"Received Goal Request With ID: "<< goal->task_number);
            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        } 

        //This will process the goal and then send an accept response, but while we're checking if we want to accept it we already start with the execution of the action
        //So that we don't take alot of time to get back to the client.
        void AcceptedCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle){
            //Create a thread to start the execution in parallel with the acceptance
            std::thread{std::bind(&TaskServer::execute ,this, _1), goal_handle}.detach();
        }

        void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle){
            RCLCPP_INFO(get_logger(),"Executing goal");

            auto arm_move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(),"arm");
            auto gripper_move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(),"gripper");

            std::vector<double> arm_joint_goal;
            std::vector<double> gripper_joint_goal;

            if(goal_handle->get_goal()->task_number == 0){
                arm_joint_goal = {0.0 , 0.0, 0.0};
                gripper_joint_goal = {-0.7,0.7};
            }
            else if(goal_handle->get_goal()->task_number == 1){
                arm_joint_goal = {-1.14 , -0.6, -0.07};
                gripper_joint_goal = {0.0,0.0};
            }
            else if(goal_handle->get_goal()->task_number == 2){
                arm_joint_goal = {-1.57 , 0.0, -1.0};
                gripper_joint_goal = {0.0,0.0};
            }
            else{
                RCLCPP_ERROR(get_logger(),"INVALID TASK NUMBER");
                return;
            }

            bool arm_within_bounds = arm_move_group.setJointValueTarget(arm_joint_goal);
            bool gripper_within_bounds = gripper_move_group.setJointValueTarget(gripper_joint_goal);

            if(!arm_within_bounds | !gripper_within_bounds){
                RCLCPP_WARN(get_logger(),"UNREACHABLE POSITION");
                return;
            }

            moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
            moveit::planning_interface::MoveGroupInterface::Plan gripper_plan;

            bool arm_plan_succ = arm_move_group.plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS;
            bool gripper_plan_succ = gripper_move_group.plan(gripper_plan) == moveit::core::MoveItErrorCode::SUCCESS;

            if(arm_plan_succ && gripper_plan_succ){
                RCLCPP_INFO(get_logger(),"Planning succeeded, Executing Trajectory...");
                arm_move_group.execute(arm_plan);
                gripper_move_group.execute(gripper_plan);
            }
            else{
                RCLCPP_ERROR(get_logger(),"Planning Failed, canceling Execution...");
                return;
            }

            auto result =  std::make_shared<arduinobot_msgs::action::ArduinobotTask::Result>();
            result->success = true;
            goal_handle->succeed(result);
            RCLCPP_INFO(get_logger(),"Goal Succeeded");

        }

        rclcpp_action::CancelResponse CancelCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle){
            RCLCPP_INFO(get_logger(),"Received request to cancel GOAL");

            //the shared from this is sharing the current node with the moveit moving groups
            auto arm_move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(),"arm");
            auto gripper_move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(),"gripper");

            arm_move_group.stop();
            gripper_move_group.stop();
            return rclcpp_action::CancelResponse::ACCEPT;
        }
};
}

RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_remote::TaskServer);