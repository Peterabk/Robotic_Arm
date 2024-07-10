import rclpy
import sys
from rclpy.node import Node
from arduinobot_msgs.srv import AddTwoInts


class SimpleServiceClient(Node):
    def __init__(self, a, b):
        super().__init__("simple_service_client")

        self.client_ = self.create_client(AddTwoInts, "add_two_ints")

        #make sure that the server is running before we send any new requests, so we'll wait for 1s
        while not self.client_.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("Service not available, waiting more...")

        #Now the req variable is the req in the AddTwoints, so now the req var has access to a and b
        self.req = AddTwoInts.Request()
        self.req.a = a
        self.req.b = b

        #taking the future value of the request ?!
        self.future = self.client_.call_async(self.req)
        self.future.add_done_callback(self.responseCallback)

    def responseCallback(self, future):
        self.get_logger().info("Service Response %d" % future.result().sum)

def main():
    rclpy.init()

    if len(sys.argv) != 3:
        print("Wrong number of arguments! Usage: simple_service_client A B")
        return -1
    
    simpleserviceclient = SimpleServiceClient(int(sys.argv[1]),int(sys.argv[2]))
    rclpy.spin(simpleserviceclient)
    simpleserviceclient.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
