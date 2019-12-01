#------------------------------------------------------------------------------
# server.py
#------------------------------------------------------------------------------


# TODO - accept multiple client connections
# allow user to select which client to talk to
# -or-
# send message to all clients


import socket               # import socket module
import sys
from _thread import *


# TODO - change from thread to threading module


s = socket.socket(          # create socket object
    socket.AF_INET,
    socket.SOCK_STREAM)

host = ''                   # symbolic name meaning all available interfaces
port = 64000                # server port
try:
    s.bind((host, port))            # bind to the port
except socket.error as msg:
    print('Bind failed. Error code : ' + str(msg[0]) + ' Message ' + msg[1])
    sys.exit()

s.listen(10)                        # wait for client connection

def clientthread(conn):             # function for handling connections
    
    while True:                     # infinite loop
        
        ch = getch()                # get character
        if ch != b'\xff':
            
            print(ch)
            conn.sendall(ch)        # send character to client

            if ch == ascii_esc:
                print("exit\n")
                break               # exit loop

    conn.close()

from getch import getch

ascii_esc = b'\x1b'                 # ASCII Esc

while True:                         # keep talking with clients
    conn, addr = s.accept()         # wait to accept a connection - blocking
    print('Connected with ' + addr[0] + ':' + str(addr[1]))
    start_new_thread(
	    clientthread,               # function name to run
		(conn,))                    # tuple of arguments

s.close

#------------------------------------------------------------------------------
