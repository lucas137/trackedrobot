#------------------------------------------------------------------------------
# robot_client.py
#------------------------------------------------------------------------------
#!/usr/bin/python3

from __future__ import print_function

import time     # time-related functions
import socket   # networking interface

# Motor command format "m1,m2"
# Value of 0 is motor stopped
# 10 is forward max speed
# -10 is reverse max speed
from pololu_drv8835_rpi import motors, MAX_SPEED

#------------------------------------------------------------------------------

robot = 1                # ID number
host  = '192.168.2.100'  # remote host
port  = 64000            # server port

#------------------------------------------------------------------------------

class RobotClient:
    def __init__(self, robot):
        self.id     = robot                 # robot ID number
        self.name   = 'robot:'+str(robot)   # robot name
        self.ack    = 'ack:'+str(robot)     # acknowledge message
        self.motors = motors                # motor driver
        self.sock   = None                  # socket
        print("robot", self.id, sep=' ')    # print robot ID
        self.motors.setSpeeds(0,0)          # make sure motors are stopped
    
    def connect(self, host, port):
        while self.sock == None:
            try:
                print("connect...")
                # create socket object and connect to server
                # timeout = None (blocking mode)
                self.sock = socket.create_connection((host, port), None)
                # send robot ID to server
                self.send_str(self.name)
                print(self.name)
                print("connected")
            except:
                print("timeout")

    def disconnect(self):
        self.motors.setSpeeds(0,0)      # stop motors
        if self.sock != None:
            self.sock.close()           # close socket
            self.sock = None
            print("connection closed")

    def is_connected(self):
        return (self.sock != None)

    def send_str(self, string):
        self.sock.sendall(string.encode('utf-8'))

    #def receive_bytes(self):
    #    try:
    #        byte_array = self.sock.recv(1024)   # receive message from server
    #        #print("received", len(byte_array), "bytes", sep=' ')
    #        #print("recv: ", byte_array)    # debug
    #        return byte_array
    #    except:
    #        print("socket.recv error")
    #        return b''

    def receive_str(self):
        try:
            byte_array = self.sock.recv(1024)   # receive message from server
            return byte_array.decode('utf-8')
        except:
            print("socket.recv error")
            return ''

    def motor_command(self, string):
        try:
            motor_values = msg.split(',')
        except:
            print("string.split error")
        else:
            motor_1 = int((MAX_SPEED * int(motor_values[0]))/10)
            motor_2 = int((MAX_SPEED * int(motor_values[1]))/10)
            self.motors.setSpeeds(motor_1, motor_2)
            print("motors:", motor_1, ',', motor_2, sep=' ')

#------------------------------------------------------------------------------

client = RobotClient(robot)

# To break loop: Ctrl + Shift + \

# socket connection loop
while True:
    client.connect(host, port)      # connect to server
  
    # message loop
    while client.is_connected():
        recv_str = client.receive_str()     # receive message from server
        if not recv_str:                    # check if no bytes received
            client.disconnect()             # stop motors and close connection
            break

        for msg in recv_str.split():
            if msg == 'close':              # server closed
                client.disconnect()         # stop motors and close connection
                break
            elif msg == 'syn':              # handshake synchronize message
                client.send_str(client.ack) # send acknowledge response
                print("syn -> ack")
            else:
                client.motor_command(msg)

client.disconnect()         # stop motors and close connection
print("exit")

#------------------------------------------------------------------------------
