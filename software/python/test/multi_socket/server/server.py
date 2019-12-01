#------------------------------------------------------------------------------
# server.py
#------------------------------------------------------------------------------

import socket
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
    ascii_esc = b'\x1b'             # ASCII Esc

    while True:                     # main loop

        # get the list sockets
        read_sockets,write_sockets,error_sockets = select.select(
            conn_list, [], [])
            
        ch = getch()                # get character - blocking
        if ch != b'\xff':
            print(ch)
    
            for sock in read_sockets:

                # new connection received through server_socket
                if sock == s:
                    sockfd, addr = s.accept()   # accept connection
                    conn_list.append(sockfd)    # add to list of connections
                    print("client (%s, %s) connected" % addr)
                    sockfd.sendall(ch)          # send character to client
                 
                # send data to client
                else:
                    try:
                        sock.sendall(ch)    # send character to client
                    except:
                        # client disconnected, so remove from socket list
                        print("client (%s, %s) is offline" % addr)
                        sock.close()
                        conn_list.remove(sock)
                        continue

            if ch == ascii_esc:
                print("exit\n")
                break               # exit loop

    s.close()                       # close socket

#------------------------------------------------------------------------------
