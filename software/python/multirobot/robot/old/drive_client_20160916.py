#------------------------------------------------------------------------------
# drive_client.py
#------------------------------------------------------------------------------

from __future__ import print_function

import socket                       # import socket module

s = socket.socket(                  # create socket object
    socket.AF_INET,
    socket.SOCK_STREAM)

robot_id   = 1
robot_name = 'robot:'+str(robot_id)
robot_ack  = 'ack:'+str(robot_id)

host = '192.168.2.100'              # remote host
#host = '172.21.36.53'               # remote host
#host = socket.gethostname()         # local machine name
port = 64000                        # server port

s.connect((host, port))                 # establish connection with server
s.sendall(robot_name.encode('utf-8'))   # send message to server

#------------------------------------------------------------------------------

from pololu_drv8835_rpi import motors, MAX_SPEED

# Motor command format "m1,m2"
# Value of 0 is motor stopped
# 10 is forward max speed
# -10 is reverse max speed

#------------------------------------------------------------------------------

while True:                         # main loop

    byte_array = s.recv(1024)       # receive message from server
    #print("recv: ", byte_array)     # debug
    recv_str = byte_array.decode('utf-8')

    if recv_str.find('close') > -1:   # close connection and stop client
      print("close")
      break                           # exit loop
  
    for msg in recv_str.split():
      if msg == 'syn':                        # handshake synchronize message
        print("syn -> ack")
        s.sendall(robot_ack.encode('utf-8'))  # send acknowledge response
      else:
        try:
          motor_values = msg.split(',')
        except:
          print("error")
        else:
          motor_1 = int((MAX_SPEED * int(motor_values[0]))/10)
          motor_2 = int((MAX_SPEED * int(motor_values[1]))/10)
          motors.setSpeeds(motor_1, motor_2)
          print("motors: ", motor_1, ',', motor_2, sep='')

motors.setSpeeds(0, 0)    # stop motors
s.close()                 # close the connection
print("exit")

#------------------------------------------------------------------------------
