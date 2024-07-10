from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable, IncludeLaunchDescription
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
import os
from os import pathsep
from ament_index_python.packages import get_package_share_directory, get_package_prefix

def generate_launch_description():

    arduinobot_description = get_package_share_directory("arduinobot_description")
    arduinobot_description_prefix = get_package_prefix("arduinobot_description")

    #now the model_path has the full path of the models in the arduinobot_description package and the share in the same package
    model_path = os.path.join(arduinobot_description, "models")
    model_path += pathsep + os.path.join(arduinobot_description_prefix, "share")

    #now we have an env variable of the name "GAZEBO_MODEL_PATH" with the content of "model_path"
    env_variable = SetEnvironmentVariable("GAZEBO_MODEL_PATH", model_path)

    #we're taking the model and what's inside of it, like the path and all information inside of it, the get_package thingy is just fancy stuff
    model_arg = DeclareLaunchArgument(
        name="model", 
        default_value=os.path.join(arduinobot_description, "urdf","arduinobot.urdf.xacro"),
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

    #This will create a new gazebo server
    start_gazebo_server = IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(
        get_package_share_directory("gazebo_ros"), "launch", "gzserver.launch.py"
    )))

    #This will create a new gazebo client
    start_gazebo_client = IncludeLaunchDescription(PythonLaunchDescriptionSource(os.path.join(
        get_package_share_directory("gazebo_ros"), "launch", "gzclient.launch.py"
    )))

    #This will spawn our robot inside of gazebo
    spawn_robot = Node(
        package="gazebo_ros",
        executable="spawn_entity.py",
        arguments=["-entity", "arduinobot", "-topic", "robot_description"],
        output="screen"
    )
    return LaunchDescription([
        env_variable,
        model_arg,
        robot_state_publisher,
        start_gazebo_server,
        start_gazebo_client,
        spawn_robot
    ])