#------------------------------------------------------------------------------
# client.py
#------------------------------------------------------------------------------

from __future__ import print_function

import socket                       # import socket module

s = socket.socket(                  # create socket object
    socket.AF_INET,
    socket.SOCK_STREAM)

#host = '192.168.1.9'                # remote host
#host = '172.25.39.81'               # remote host
host = socket.gethostname()         # get local machine name
port = 64000                        # server port

s.connect((host, port))             # establish connection with server
s.sendall(b"hello")                 # send message to server

#------------------------------------------------------------------------------

ascii_esc = b'\x1b'                 # ASCII Esc

#------------------------------------------------------------------------------

while True:                         # main loop
    data = s.recv(1024)             # receive message from server
    print(data)
    if data == ascii_esc:           # close connection and stop client
        print("exit")
        break                       # exit loop

s.close()                           # close socket

#------------------------------------------------------------------------------
