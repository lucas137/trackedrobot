import msvcrt               # Windows only
    
def getch():
    if msvcrt.kbhit():
       return ord(msvcrt.getch())
    return 0


'''
while True:                 # test
    ch = getch()
    if ch != b'\xff':
        if ch == b'\x1b':    # ASCII Esc
            print("exit\n")
            break
        else:
            print(ch)
'''
