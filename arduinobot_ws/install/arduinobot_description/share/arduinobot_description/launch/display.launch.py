from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory

#this function will return the nodes that we want the launch file to launch once we activate it
def generate_launch_description():
    #we're taking the model and what's inside of it, like the path and all information inside of it, the get_package thingy is just fancy stuff
    model_arg = DeclareLaunchArgument(
        name="model", 
        default_value=os.path.join(get_package_share_directory("arduinobot_description"), "urdf","arduinobot.urdf.xacro"),
        description="Absolute path to the robot URDF file"
        )
    
    #The Command is used to let the ParameterValue know that the file is in xacro and needs to be transformed to urdf and the LAunchconfig is reading the model mentioned above
    #this Command will create an actual command in the terminal with the word xacro first and then the LaunchConfig model thingy after it
    robot_description = ParameterValue(Command(["xacro ", LaunchConfiguration("model")]))

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description" : robot_description}]
    )

    joint_state_publisher_gui = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui"
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        #we want to see the log of this node in the terminal or all the node in the terminal
        output="screen",
        arguments=["-d", os.path.join(get_package_share_directory("arduinobot_description"), "rviz", "display.rviz")]
    )
    return LaunchDescription([
        model_arg,
        robot_state_publisher,
        joint_state_publisher_gui,
        rviz_node
    ])