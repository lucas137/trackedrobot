#------------------------------------------------------------------------------
import socket

name = 'www.wayne.edu'
#name = 'wwwx.wayne.edu'
#name = 'rpi-bp-01'

try:
    host = socket.gethostbyname(name)
    print('hostname: ' + name)
    print('resolves: ' + host)
except socket.gaierror, err:
    print("cannot resolve hostname: " + name)
    print(err)

#------------------------------------------------------------------------------
