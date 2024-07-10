import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class SimplePublisher(Node):
    def __init__(self):
        #initalize the constructor of the Node with the name
        super().__init__("simple_publisher")

        #Create the publisher
        #We need to put the proper message type/Name of topic/buffer size in messages
        self.pub = self.create_publisher(String,"chatter",10)
        #Nbr of messages published in the topic
        self.counter = 0
        #Frequency of messages ex: 1 message/second
        self.frequency = 1.0
        self.get_logger().info("Publishing at %d Hz" % self.frequency)
        #it's a timer with the frequuency of 1hz where it calls the callback that we will define
        self.timer = self.create_timer(self.frequency,self.timerCallback)

    def timerCallback(self):
        #create a new message to be published
        msg = String()
        msg.data = "Hello Ros2 - counter: %d" % self.counter
        self.pub.publish(msg)
        self.counter += 1

def main():
    rclpy.init()
    simple_publisher = SimplePublisher()
    rclpy.spin(simple_publisher)
    simple_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()