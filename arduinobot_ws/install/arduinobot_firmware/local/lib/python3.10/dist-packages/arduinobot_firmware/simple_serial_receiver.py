#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import serial

class SimpleSerialReceiver(Node):
    def __init__(self):
        #initalize the constructor of the Node with the name
        super().__init__("simple_serial_receiver")

        self.declare_parameter("port", "/dev/ttyUSB0")
        self.declare_parameter("baudrate", 115200)

        self.port_ = self.get_parameter("port").value
        self.baudrate_ = self.get_parameter("baudrate").value

        #Create the publisher
        #We need to put the proper message type/Name of topic/buffer size in messages
        self.pub = self.create_publisher(String,"serial_receiver",10)
        #Frequency of messages ex: 1 message/second
        self.frequency = 0.01

        self.arduino_ = serial.Serial(port=self.port_, baudrate=self.baudrate_, timeout=0.1 )
        #it's a timer with the frequuency of 1hz where it calls the callback that we will define
        self.timer = self.create_timer(self.frequency,self.timerCallback)

    def timerCallback(self):
        #create a new message to be published
        if rclpy.ok() and self.arduino_.is_open:
            data = self.arduino_.readline()

            try:
                data.decode("utf-8")
            except:
                return
            msg = String()
            msg.data = str(data)
            self.pub.publish(msg)

def main():
    rclpy.init()
    simple_serial_receiver = SimpleSerialReceiver()
    rclpy.spin(simple_serial_receiver)
    simple_serial_receiver.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()