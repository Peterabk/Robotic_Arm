import rclpy
from rclpy import Node
from rcl_interfaces.msg import SetParametersResult
from rclpy.parameter import Parameter

class SimpleParameter(Node):
    def __init__(self):
        super().__init__("simple_parameter")

        #you give it a name and a default value
        #an important feature of the parameters that they can be configured and changed when the node is running
        self.declare_parameter("simple_int_param", 28)
        self.declare_parameter("simple_string_param", "Peter")

        self.add_on_set_parameters_callback(self.paramChangeCallback)

    #This function receives an argument which is params from the call back event
    #it also returns ParameterResults
    def paramChangeCallback(self, params):
        result = SetParametersResult()

        for param in params:
            if(param.name == "simple_int_param" and param.type_ == Parameter.Type.INTEGER):
                self.get_logger().info("Param simple_int_param changed! New value is: %d" % param.value)
                result.successful = True

            if(param.name == "simple_string_param" and param.type_ == Parameter.Type.STRING):
                self.get_logger().info("Param simple_string_param changed! New value is %s" % param.value)
                result.successful = True

        return result
    
    def main():
        rclpy.init()
        simple_parameter = SimpleParameter()
        rclpy.spin(simple_parameter)
        simple_parameter.destroy_node()
        rclpy.shutdown()

    if __name__ == '__main__':
        main()
