#include "arduinobot_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>

namespace arduinobot_controller
{
    ArduinobotInterface::ArduinobotInterface(){

    }

    ArduinobotInterface::~ArduinobotInterface(){
        if(arduino_.IsOpen()){
            try{
                arduino_.Close();
            }catch(...){
                RCLCPP_INFO_STREAM(rclcpp::get_logger("ArduinobotInterface"),"Something went wrong while closing the connection with the port: " << port_);
            }
        }
    }

    CallbackReturn ArduinobotInterface::on_init(const hardware_interface::HardwareInfo &hardware_info){
        CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);

        if(result != CallbackReturn::SUCCESS)
        {
            return result;
        }

        try{
            //info is defined in the System interface of the hardware library
            port_ = info_.hardware_parameters.at("port");
        }
        catch(const std::out_of_range &e){
            RCLCPP_FATAL(rclcpp::get_logger("ArduinobotInterface"), "No Serial Port provided! Aborting!");
            return CallbackReturn::FAILURE;
        }

        //reserve a space in the memory since we know how much space will it be taking
        position_commands_.reserve(info_.joints.size());
        position_states_.reserve(info_.joints.size());
        prev_position_commands_.reserve(info_.joints.size());

        return CallbackReturn::SUCCESS;
    }

    //create a vector of objects that are of type state interface that are still declared in the hardware inteface level
    std::vector<hardware_interface::StateInterface> ArduinobotInterface::export_state_interfaces(){
        std::vector<hardware_interface::StateInterface> state_interfaces;

        //each joint gives it's position in a form of a position state interface, so we loop through them to get their positions respectivelly
        for(size_t i=0; i<info_.joints.size(); i++){
            state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
            //it takes the joint at index i and it's name, and from the library of the hardware_interface type ..., we get the postion and we add it to the new variable
            //by reference in the index that is i
        }

        return state_interfaces;
    }

    //communicate the interface we need to use in order to send commands for each joint
    std::vector<hardware_interface::CommandInterface> ArduinobotInterface::export_command_interfaces(){
        std::vector<hardware_interface::CommandInterface> command_interfaces;

        for(size_t i=0; i<info_.joints.size(); i++){
            command_interfaces.emplace_back(hardware_interface::CommandInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_commands_[i]));
            //it takes the joint at index i and it's name, and from the library of the hardware_interface type ..., we get the type of interface we want to use: postion and we add it to the new variable
            //by reference in the index that is i
        }
        return command_interfaces;
    }

    CallbackReturn ArduinobotInterface::on_activate(const rclcpp_lifecycle::State &previous_state){
        RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Starting the robot hardware...");
        position_commands_ = {0.0 ,0.0 ,0.0 ,0.0};
        prev_position_commands_ = {0.0 ,0.0 ,0.0 ,0.0};
        position_states_ = {0.0 ,0.0 ,0.0 ,0.0};

        try{
            arduino_.Open(port_);
            arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        }
        catch(...){
            RCLCPP_INFO_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Something went wrong when intercating with the port: "<<port_);
            return CallbackReturn::FAILURE;
        }

        RCLCPP_INFO_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Hardware started, Ready to take command on the port: "<<port_);
        return CallbackReturn::SUCCESS;

    }

    CallbackReturn ArduinobotInterface::on_deactivate(const rclcpp_lifecycle::State &previous_state){
        RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Stopping the robot hardware...");

        if(arduino_.IsOpen()){
            try
            {
                arduino_.Close();
            }
            catch(...)
            {
                RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Something went wrong when closing the connection with the port: "<<port_);
                return CallbackReturn::FAILURE;
            }
        }
        RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Hardware Stopped");
        return CallbackReturn::SUCCESS;
    }

    hardware_interface::return_type ArduinobotInterface::read(const rclcpp::Time &time, const rclcpp::Duration &period){
        //with better motors we can have feedback on their positions.
        //here now we are doing a mock test that the postion will actually be on the command we sent it to be
        //usually we should be able to get the read back and make sure for it to get to the commanded area.
        position_states_ = position_commands_;
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type ArduinobotInterface::write(const rclcpp::Time &time, const rclcpp::Duration &period){
        //we need to check that the new command's position is different than the prev position
        //cause if it's the same the robot will still be in the same place

        if(position_commands_ == prev_position_commands_){
            return hardware_interface::return_type::OK;
        }

        std::string msg;
        int base = static_cast<int>(((position_commands_.at(0) + (M_PI/2))*180)/M_PI);
        msg.append("b");
        msg.append(std::to_string(base));
        msg.append(",");
        int shoulder = 180 - static_cast<int>(((position_commands_.at(1) + (M_PI/2))*180)/M_PI);
        msg.append("s");
        msg.append(std::to_string(shoulder));
        msg.append(",");
        int elbow = static_cast<int>(((position_commands_.at(2) + (M_PI/2))*180)/M_PI);
        msg.append("e");
        msg.append(std::to_string(elbow));
        msg.append(",");
        int gripper = static_cast<int>((-position_commands_.at(3) * 180)/M_PI);
        msg.append("g");
        msg.append(std::to_string(gripper));
        msg.append(",");

        try{
            RCLCPP_INFO_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Sending To Arduino : " << msg);
            arduino_.Write(msg);
        }
        catch(...){
            RCLCPP_ERROR_STREAM(rclcpp::get_logger("ArduinobotInterface"),"Was not able to send the message through Serial Communication using port: "<<port_);
            return hardware_interface::return_type::ERROR;
        }

        prev_position_commands_ = position_commands_;
        return hardware_interface::return_type::OK;
    }
    
}

PLUGINLIB_EXPORT_CLASS(arduinobot_controller::ArduinobotInterface, hardware_interface::SystemInterface);