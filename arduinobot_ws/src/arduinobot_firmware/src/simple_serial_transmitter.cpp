#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>

//to say that the callback function takes 1 input
using std::placeholders::_1;

class SimpleSerialTransmitter : public rclcpp::Node
{
    public:
        SimpleSerialTransmitter() : Node("simple_serial_transmitter")
        {
            //we are declaring parameters so that we can get the port and baud rate for the arduino without hardcoding it 
            declare_parameter<std::string>("port", "/dev/ttyUSB0");
            port_ = get_parameter("port").as_string();

            sub_ = create_subscription<std_msgs::msg::String>("serial_transmitter",10,std::bind(&SimpleSerialTransmitter::messageCallBack,this,_1));

            arduino_.Open(port_);
            arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        }

        ~SimpleSerialTransmitter()
        {
            arduino_.Close();
        }

        void messageCallBack(const std_msgs::msg::String &message)
        {
            RCLCPP_INFO_STREAM(get_logger(),"New message received, publishing on serial port:  - " << message.data);
            arduino_.Write(message.data);
        }

    private:
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
        std::string port_;
        LibSerial::SerialPort arduino_;

};

int main(int args, char* argv[]){
    rclcpp::init(args,argv);
    auto node = std::make_shared<SimpleSerialTransmitter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}