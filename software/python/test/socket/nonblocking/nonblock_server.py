#------------------------------------------------------------------------------

import socket               # import socket module

sock = socket.socket(       # create socket object
  socket.AF_INET,
  socket.SOCK_STREAM)

host = ''                   # symbolic name meaning all available interfaces
port = 64000                # server port
sock.bind((host, port))     # bind to the port

sock.listen(1)                      # wait for client connection
conn, addr = sock.accept()          # establish connection with client
print("connected by", addr)         # print client address

#------------------------------------------------------------------------------

from getch import getch

exit_program    = b'\x1b'           # ASCII Esc

#------------------------------------------------------------------------------

while True:                         # main loop
  ch = getch()                    # get character
  if ch != b'\xff':

    if ch == exit_program:
      print("exit\n")
      conn.sendall(b'close')  # send message to client
      break

    print(ch)
    conn.sendall(ch)

conn.close()                        # close socket

#------------------------------------------------------------------------------
