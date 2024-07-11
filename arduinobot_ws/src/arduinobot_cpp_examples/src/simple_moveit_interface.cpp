#include<rclcpp/rclcpp.hpp>
#include<moveit/move_group_interface/move_group_interface.h>
#include<memory>


void move_robot(const std::shared_ptr<rclcpp::Node> node){
    auto arm_move_group = moveit::planning_interface::MoveGroupInterface(node, "arm");
    auto gripper_move_group = moveit::planning_interface::MoveGroupInterface(node, "gripper");

    std::vector<double> arm_joint_goal {1.57, 0.0, 0.0};
    std::vector<double> gripper_joint_goal {-0.7, 0.7};

    bool arm_within_bound = arm_move_group.setJointValueTarget(arm_joint_goal);
    bool gripper_within_bound = gripper_move_group.setJointValueTarget(gripper_joint_goal);

    if(!arm_within_bound | !gripper_within_bound){
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"Target joint position went off limit");
        return;
    }

    moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
    moveit::planning_interface::MoveGroupInterface::Plan gripper_plan;

    bool arm_plan_success = arm_move_group.plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS;
    bool gripper_plan_success = gripper_move_group.plan(gripper_plan) == moveit::core::MoveItErrorCode::SUCCESS;

    if(arm_plan_success && gripper_plan_success){
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Planner succeeded, moving the arm and gripper");
        arm_move_group.move();
        gripper_move_group.move();
    }
    else{
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "one or more planning has failed");
        return;
    }

}

int main(int args, char* argv[]){

    rclcpp::init(args,argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("simple_moveit_interface");

    move_robot(node);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}