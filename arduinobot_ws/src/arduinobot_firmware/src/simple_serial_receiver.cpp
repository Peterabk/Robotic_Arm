#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>

#include <chrono>

using namespace std::chrono_literals;

//inhereting the Node class
class SimpleSerialReceiver : public rclcpp::Node{

public:
    SimpleSerialReceiver() : Node("simple_serial_receiver")
    {
        //Create the publisher with the name and number of message queue
        pub_ = create_publisher<std_msgs::msg::String>("serial_receiver",10);

        //binding the call back with the class instance,
        // and showing that the callback function is from the class
        timer_ = create_wall_timer(0.01s,std::bind(&SimpleSerialReceiver::timerCallBack,this));

        //we are declaring parameters so that we can get the port and baud rate for the arduino without hardcoding it 
        declare_parameter<std::string>("port", "/dev/ttyUSB0");
        port_ = get_parameter("port").as_string();
        arduino_.Open(port_);
        arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    }

    ~SimpleSerialReceiver()
    {
        arduino_.Close();
    }

    void timerCallBack()
    {
        //creating a message variable to send
        auto message = std_msgs::msg::String();
        if(rclcpp::ok() && arduino_.IsDataAvailable())
        {
            arduino_.ReadLine(message.data);
        }
        pub_->publish(message);
    }

private:
    //sharedptr is the pointer used to refer to the publisher and timer
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    LibSerial::SerialPort arduino_;
    std::string port_;
};

int main(int args, char* argv[])
{
    rclcpp::init(args,argv);
    auto node = std::make_shared<SimpleSerialReceiver>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0; 
}