#------------------------------------------------------------------------------
# robot_server.py
#------------------------------------------------------------------------------

import socket               # Import socket module
import select

if __name__ == "__main__":
      
    host = ''                       # symbolic name meaning
                                    # all available interfaces
    port = 64000                    # private server port
    conn_list = []                  # list of socket clients
    
    s = socket.socket(              # create socket object
        socket.AF_INET,
        socket.SOCK_STREAM)
        
    try:
        s.bind((host, port))        # bind to the port
    except socket.error as msg:
        print('bind failed. error code : ' +
            str(msg[0]) + ' message ' + msg[1])
        sys.exit()

    s.listen(10)                    # wait for client connection
    conn_list.append(s)             # add server socket to list of connections
 
    print("server started on port " + str(port))

#------------------------------------------------------------------------------

    from getch import getch

    exit_program    = b'\x1b'       # ASCII Esc

    # key map
    ''' 7 8 9 -- forward
        4 5 6 -- stop
        1 2 3 -- reverse
        | | |
        | | right
        | straight
        left
    '''
    reverse_left    = b'\x31'       # ASCII '1'
    reverse         = b'\x32'       # ASCII '2'
    reverse_right   = b'\x33'       # ASCII '3'

    pivot_left      = b'\x34'       # ASCII '4'
    stop            = b'\x35'       # ASCII '5'
    pivot_right     = b'\x36'       # ASCII '6'

    forward_left    = b'\x37'       # ASCII '7'
    forward         = b'\x38'       # ASCII '8'
    forward_right   = b'\x39'       # ASCII '9'

    shutdown        = b'\x73'       # ASCII 's'

#------------------------------------------------------------------------------

    while True:                     # main loop

        # get the list sockets
        read_sockets,write_sockets,error_sockets = select.select(
            conn_list, [], [])
            
        ch = getch()                # get character - blocking
        
        if ch != b'\xff':
        
            if ch == exit_program:
                print("close")

            elif ch == stop:
                print("stop")
                msg = b'0,0'

            elif ch == forward:
                print("forward")
                msg = b'65,65'
                
            elif ch == forward_left:
                print("forward-left")
                msg = b'40,70'
                
            elif ch == forward_right:
                print("forward-right")
                msg = b'70,40'
                
            elif ch == reverse:
                print("reverse")
                msg = b'-65,-65'
                
            elif ch == reverse_left:
                print("reverse-left")
                msg = b'-40,-70'
                
            elif ch == reverse_right:
                print("reverse-right")
                msg = b'-70,-40'
                
            elif ch == pivot_left:
                print("pivot-left")
                msg = b'-75,75'
                
            elif ch == pivot_right:
                print("pivot-right")
                msg = b'75,-75'

            elif ch == shutdown:
                print("shutdown")
                msg = b'shutdown'

            else:
                print("invalid input")
                msg = b'0,0'
        
            for sock in read_sockets:

                # new connection received through server_socket
                if sock == s:
                    sockfd, addr = s.accept()   # accept connection
                    conn_list.append(sockfd)    # add to list of connections
                    print("client (%s, %s) connected" % addr)
                    sockfd.sendall(msg)         # send character to client
                 
                # send data to client
                else:
                    try:
                        sock.sendall(msg)       # send character to client
                    except:
                        # client disconnected, so remove from socket list
                        print("client (%s, %s) is offline" % addr)
                        sock.close()
                        conn_list.remove(sock)
                        continue

            if ch == exit_program:
                print("exit")
                break               # exit loop

    s.close()                       # close socket

#------------------------------------------------------------------------------
