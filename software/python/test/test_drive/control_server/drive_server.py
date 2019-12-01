#------------------------------------------------------------------------------
# drive_server.py
#------------------------------------------------------------------------------

import socket               # Import socket module

s = socket.socket(          # Create socket object
    socket.AF_INET,
    socket.SOCK_STREAM)

host = ''                   # Symbolic name meaning all available interfaces
port = 64000                # Server port
s.bind((host, port))        # Bind to the port

s.listen(1)                         # Wait for client connection
conn, addr = s.accept()             # Establish connection with client
print("Connected by", addr)         # Print client address

#------------------------------------------------------------------------------

from getch import getch

exit_program    = b'\x1b'           # ASCII Esc

# key map
''' 7 8 9 -- forward
    4 5 6 -- stop
    1 2 3 -- reverse
    | | |
    | | right
    | straight
    left
'''
reverse_left    = b'\x31'           # ASCII '1'
reverse         = b'\x32'           # ASCII '2'
reverse_right   = b'\x33'           # ASCII '3'

skid_left       = b'\x34'           # ASCII '4'
stop            = b'\x35'           # ASCII '5'
skid_right      = b'\x36'           # ASCII '6'

forward_left    = b'\x37'           # ASCII '7'
forward         = b'\x38'           # ASCII '8'
forward_right   = b'\x39'           # ASCII '9'

#------------------------------------------------------------------------------

while True:                         # main loop
    ch = getch()                    # get character
    if ch != b'\xff':

        if ch == exit_program:
            print("exit\n")
            conn.sendall(b'close')  # Send message to client
            break

        if ch == stop:
            print("stop")
            conn.sendall(b'0,0')

        elif ch == forward:
            print("forward")
            conn.sendall(b'1,1')
            
        elif ch == forward_left:
            print("forward-left")
            conn.sendall(b'0,1')
            
        elif ch == forward_right:
            print("forward-right")
            conn.sendall(b'1,0')
            
        elif ch == reverse:
            print("reverse")
            conn.sendall(b'-1,-1')
            
        elif ch == reverse_left:
            print("reverse-left")
            conn.sendall(b'0,-1')
            
        elif ch == reverse_right:
            print("reverse-right")
            conn.sendall(b'-1,0')
            
        elif ch == skid_left:
            print("skid-left")
            conn.sendall(b'-1,1')
            
        elif ch == skid_right:
            print("skid-right")
            conn.sendall(b'1,-1')

conn.close()                        # Close socket

#------------------------------------------------------------------------------
