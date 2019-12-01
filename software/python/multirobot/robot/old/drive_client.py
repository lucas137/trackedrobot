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

host = '192.168.2.100'              # remote host
#host = '172.25.39.81'               # remote host
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
    print("recv: ", byte_array)
    recv_str = byte_array.decode('utf-8')

    if byte_array == b'close':      # close connection and stop client
        break                       # exit loop
    try:
        motor_values = recv_str.split(',')
    except:
        print("error")
        break
    else:
        motor_1 = int((MAX_SPEED * int(motor_values[0]))/10)
        motor_2 = int((MAX_SPEED * int(motor_values[1]))/10)
        motors.setSpeeds(motor_1, motor_2)
        print("motors: ", motor_1, ',', motor_2, sep='')

motors.setSpeeds(0, 0)              # stop motors
s.close()                           # close the connection
print("exit")

#------------------------------------------------------------------------------
