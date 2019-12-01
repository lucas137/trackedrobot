#------------------------------------------------------------------------------
# robot-client-3.py
#------------------------------------------------------------------------------
#!/usr/bin/python3

# Robot client software

# Note: from __future__ imports must occur at the beginning of the file
from __future__ import print_function

debug = None
#debug = True    # off-board client emulator

import time     # time-related functions
import socket   # networking interface
import sys      # system-specific parameters

# Motor command format "m1,m2"
# Value of 0 is motor stopped
# 100 is maximum speed forward
# -100 is maximum speed backward
MAX_INPUT =  100

if debug:
    MAX_SPEED = 480
else:
    from pololu_drv8835_rpi import motors, MAX_SPEED

#------------------------------------------------------------------------------

robot   = 3                 # ID number
host    = '192.168.2.100'   # remote host
port    = 64000             # server port
timeout = 1                 # connection timeout in seconds
                            # blocking mode:  timeout = None
if debug:
    host = '192.168.1.3'    # remote host for debugging

if len(sys.argv) > 1:
	host = sys.argv[1]
if len(sys.argv) > 2:
	port = sys.argv[2]

print('remote host:', host, sep=' ')
print('port number:', port, sep=' ')

#------------------------------------------------------------------------------

class RobotClient:
    def __init__(self, robot):
        self.id     = robot                 # robot ID number
        self.name   = 'robot:'+str(robot)   # robot name
        self.ack    = 'ack:'+str(robot)     # acknowledge message
        if not debug:
            self.motors = motors            # motor driver
        self.sock   = None                  # socket
        print("robot", self.id, sep=' ')    # print robot ID
        if not debug:
            self.motors.setSpeeds(0,0)      # make sure motors are stopped
    
    def connect(self, host, port):
        while self.sock == None:
            try:
                print("robot", self.id, "connect...", sep=' ')
                # create socket object and connect to server
                self.sock = socket.create_connection((host, port), timeout)
                # send robot ID to server
                self.send_str(self.name)
                print(self.name)
                print("robot", self.id, "connected")
            except:
                print("robot", self.id, "timeout", sep=' ')

    def disconnect(self):
        if not debug:
            self.motors.setSpeeds(0,0)      # stop motors
        if self.sock != None:
            self.sock.close()               # close socket
            self.sock = None
            print("robot", self.id, "connection closed", sep=' ')

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
            print("robot", self.id, "socket.recv error", sep=' ')
            return ''

    def motor_command(self, string):
        try:
            motor_values = msg.split(',')
        except:
            print("robot", self.id, "string.split error", sep=' ')
        else:
            motor_1 = int((MAX_SPEED * int(motor_values[0])) / MAX_INPUT)
            motor_2 = int((MAX_SPEED * int(motor_values[1])) / MAX_INPUT)
            if motor_1 >  MAX_SPEED: motor_1 =  MAX_SPEED
            if motor_1 < -MAX_SPEED: motor_1 = -MAX_SPEED
            if motor_2 >  MAX_SPEED: motor_2 =  MAX_SPEED
            if motor_2 < -MAX_SPEED: motor_2 = -MAX_SPEED
            if not debug:
                self.motors.setSpeeds(motor_1, motor_2)
            print("robot", self.id, "motors:", motor_1, ',', motor_2, sep=' ')

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
                print("close")
                client.disconnect()         # stop motors and close connection
                break
            elif msg == 'shutdown':
                client.disconnect()
                print("shutdown")
                sys.exit()
            elif msg == 'syn':              # handshake synchronize message
                client.send_str(client.ack) # send acknowledge response
                print("syn -> ack")
            else:
                client.motor_command(msg)

client.disconnect()         # stop motors and close connection
print("exit")

#------------------------------------------------------------------------------
