#------------------------------------------------------------------------------

from __future__ import print_function

import sys
import socket                   # import socket module
from time import sleep

sock = socket.socket(           # create socket object
  socket.AF_INET,
  socket.SOCK_STREAM)

#host = '192.168.1.9'           # remote host
#host = '172.25.39.81'          # remote host
host = socket.gethostname()     # get local machine name
port = 64000                    # server port
sock.connect((host, port))      # establish connection with server

sock.settimeout(0.03)           # timeout on blocking socket operations,
                                # floating point number expressing seconds

#------------------------------------------------------------------------------

while True:                         # main loop

  try:
    msg = sock.recv(1024)           # receive message from server
  except socket.timeout as e:
    err = e.args[0]
    # this next if/else is a bit redundant, but illustrates how the
    # timeout exception is setup
    if err == 'timed out':
      #sleep(1)
      print('recv timed out, retry later')
      continue
    else:
      print(e)
      sys.exit(1)
  except socket.error as e:
    # Something else happened, handle error, exit, etc.
    print(e)
    sys.exit(1)
  else:
    if msg == b'close':
      print("exit")
      break                           # exit loop
    print(msg)
    
sock.close()                        # close the connection

#------------------------------------------------------------------------------
