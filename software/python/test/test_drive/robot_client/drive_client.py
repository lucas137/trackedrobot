#------------------------------------------------------------------------------
# drive_client.py
#------------------------------------------------------------------------------

from __future__ import print_function

import socket               # Import socket module

s = socket.socket(          # Create socket object
    socket.AF_INET,
    socket.SOCK_STREAM)

#host = '192.168.1.9'               # The remote host
host = '172.25.39.81'              # The remote host
#host = socket.gethostname()         # Get local machine name
port = 64000                        # Server port

s.connect((host, port))             # Establish connection with server

#------------------------------------------------------------------------------

from pololu_drv8835_rpi import motors, MAX_SPEED

max_speed = 50

# Motor command format 'm1,m2'
# Value of 0 is motor stopped
# 1 is forward max speed
# -1 is reverse max speed

#------------------------------------------------------------------------------

while True:                         # Main loop
    data = s.recv(1024)             # Receive message from server
    print(data)
    if data == 'close':             # Close connection and stop client
        print("exit")
        break                       # Exit loop
    try:
        motor_values = data.split(',')
    except:
        print("error")
        break
    else:
        motors.setSpeeds(
            int(max_speed * float(motor_values[0])),     # Motor 1
            int(max_speed * float(motor_values[1])))     # Motor 2

motors.setSpeeds(0, 0)              # Stop motors
s.close()                           # Close the connection

#------------------------------------------------------------------------------
