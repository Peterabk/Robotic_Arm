#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import EulerToQuaternion,QuaternionToEuler
from tf_transformations import quaternion_from_euler,euler_from_quaternion

class AnglesConverter(Node):
    def __init__(self):
        super().__init__("angle_conversion_service_server")

        #creates the service object also takes the msg as input
        self.euler_to_quaternion = self.create_service(EulerToQuaternion, "euler_to_quaternion", self.EulerToQuaternion)
        self.quaternion_to_euler = self.create_service(QuaternionToEuler, "quaternion_to_euler", self.QuaternionToEuler)
        self.get_logger().info("Angle Conversion Services Ready")

    def EulerToQuaternion(self, req, res):
        self.get_logger().info("New Message Received, Euler to Quaternion Conversion roll: %f, pitch: %f, yaw: %f" % (req.roll, req.pitch ,req.yaw))
        (res.x, res.y, res.z, res.w)=quaternion_from_euler(req.roll, req.pitch ,req.yaw)
        self.get_logger().info("Returning Quaternion Values:  x= %f, y=%f, z=%f, w=%f " % (res.x, res.y, res.z, res.w))
        return res
    
    def QuaternionToEuler(self, req, res):
        self.get_logger().info("New Message Received, Quaternion to Euler Conversion x: %f, y: %f, z: %f, w: %f" % (req.x, req.y, req.z, req.w))
        (res.roll, res.pitch, res.yaw)=euler_from_quaternion(req.x, req.y, req.z, req.w)
        self.get_logger().info("Returning Euler Values:  roll= %f, pitch= %f, yaw= %f " % (res.roll, res.pitch, res.yaw))
        return res
    
def main():
    rclpy.init()
    angles_converter = AnglesConverter()
    rclpy.spin(angles_converter)
    angles_converter.destroy_node()
    rclpy.shutdown()
    
if __name__ == '__main__':
    main()