#include<rclcpp/rclcpp.hpp>
#include<memory>
#include<arduinobot_msgs/srv/euler_to_quaternion.hpp>
#include<arduinobot_msgs/srv/quaternion_to_euler.hpp>
#include<tf2/utils.h>

using namespace std::placeholders;

class AnglesConverter : public rclcpp::Node{

public:
    AnglesConverter(): Node("angles_conversion_service"){

        euler_to_quaternion = create_service<arduinobot_msgs::srv::EulerToQuaternion>("euler_to_quaternion",std::bind(&AnglesConverter::EulerToQuaternionCallback,this, _1,_2 ));
        quaternion_to_euler = create_service<arduinobot_msgs::srv::QuaternionToEuler>("quaternion_to_euler",std::bind(&AnglesConverter::QuaterniontToEulerCallback,this, _1,_2 ));
        RCLCPP_INFO(get_logger(), "Angle Conversion services are ready");
    }

private:

    rclcpp::Service<arduinobot_msgs::srv::EulerToQuaternion>::SharedPtr euler_to_quaternion;
    rclcpp::Service<arduinobot_msgs::srv::QuaternionToEuler>::SharedPtr quaternion_to_euler;

    void EulerToQuaternionCallback(const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Request> req, 
                                const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Response> resp){

        tf2::Quaternion q;
        RCLCPP_INFO_STREAM(get_logger(),"New Request Received Convert Euler Angles: roll: " << req->roll <<" pitch: "<< req->pitch<<" yaw: "<< req->yaw);
        q.setRPY(req->roll,req->pitch,req->yaw);
        resp->x = q.getX();
        resp->y = q.getY();
        resp->z = q.getZ();
        resp->w = q.getW();
        RCLCPP_INFO_STREAM(get_logger(), "Quaternion Angles Calculated: x: "<< resp->x <<" y: "<<resp->y<< " z: "<<resp->z<< " w: "<< resp->w);
     }

    void QuaterniontToEulerCallback(const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Request> req, 
                                const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Response> resp){

        tf2::Quaternion q(req->x,req->y,req->z,req->w);
        tf2::Matrix3x3 m(q);
        RCLCPP_INFO_STREAM(get_logger(),"New Request Received Convert Quaternion Angles: x: "<< req->x <<" y: "<<req->y<< " z: "<<req->z<< " w: "<< req->w);
        m.getRPY(resp->roll,resp->pitch,resp->yaw);
        RCLCPP_INFO_STREAM(get_logger(), "Euler Angles Calculated: roll: " << resp->roll <<" pitch: "<< resp->pitch<<" yaw: "<< resp->yaw);
     }
};

int main(int argc, char* argv[]){

    rclcpp::init(argc,argv);
    auto node = std::make_shared<AnglesConverter>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}