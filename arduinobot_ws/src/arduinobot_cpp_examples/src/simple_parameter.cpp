#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <string>
#include <vector>
#include <memory>

//To say that the callback function is anticipating only 1 argument from the event
using std::placeholders::_1;

class SimpleParameter : public rclcpp::Node{

    public:
        SimpleParameter() : Node("simple_parameter")
        {
            declare_parameter<int>("simple_int_param", 28);
            declare_parameter<std::string>("simple_string_param", "peter");

            param_callback_handle_ = add_on_set_parameters_callback(std::bind(&SimpleParameter::paramChangeCallback, this, _1));
        }

    private:

        OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

        rcl_interfaces::msg::SetParametersResult paramChangeCallback(const std::vector<rclcpp::Parameter> &parameters)
        {
            rcl_interfaces::msg::SetParametersResult Result;
            for(const auto& param : parameters)
            {
                if(param.get_name() == "simple_int_param" && param.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER)
                {
                    RCLCPP_INFO_STREAM(get_logger(), "Param simple_int_param changed! New value is : " << param.as_int());
                    Result.successful = true;
                }

                if(param.get_name() == "simple_string_param" && param.get_type() == rclcpp::ParameterType::PARAMETER_STRING)
                {
                    RCLCPP_INFO_STREAM(get_logger(), "Param simple_int_param changed! New value is : " << param.as_string());
                    Result.successful = true;
                }
            }
            return Result;
        }
};


int main(int args, char* argv[]){
    rclcpp::init(args,argv);

    auto node = std::make_shared<SimpleParameter>();
    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}