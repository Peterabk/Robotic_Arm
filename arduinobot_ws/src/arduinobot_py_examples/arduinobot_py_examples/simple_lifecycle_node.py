import time
import rclpy
import rclpy.executors
from rclpy.lifecycle import Node, State, TransitionCallbackReturn
from std_msgs.msg import String

class SimpleLifeCycleNode(Node):
    def __init__(slef,node_name,**kwrgs):
        super.__init__(node_name,**kwrgs)

    def on_configure(self, state: State) -> TransitionCallbackReturn:
        self.sub_ = self.create_subscription(String, "chatter", self.msgCallback, 10)
        self.get_logger().info("Lifecycle Node on_configure() called.")
        return TransitionCallbackReturn.SUCCESS
    
    def on_shutdown(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_shutdown() called.")
        return TransitionCallbackReturn.SUCCESS
    
    def on_cleanup(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_cleanup() called.")
        return TransitionCallbackReturn.SUCCESS
    
    def on_activte(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_activate() called.")
        time.sleep(2)
        return super().on_activate(state)
    
    def on_deactivte(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_deactivate() called.")
        return super().on_deactivate(state)
    
    def msgCallback(self, msg):
        current_state = self._state_machine.current_state
        if current_state[1] == "active":
            self.get_logger().info("I heard: %s " % msg.data)


def main():
    rclpy.init()
    executor = rclpy.executors.SingleThreadedExecutor()
    simple_lifecycle_node = SimpleLifeCycleNode("simple_lifecycle_node")
    executor.add_node(simple_lifecycle_node)
    try:
        executor.spin()
    except(KeyboardInterrupt, rclpy.executors.ExternalShutdownException):
        simple_lifecycle_node.destroy_node()

if __name__ == '__main__':
    main()