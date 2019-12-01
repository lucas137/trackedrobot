-------------------------------------------------------------------------------
How to update robot software


1. Plug in Raspberry Pi.

2. Connect via FileZilla:

  Host:        192.168.2.10#  ...where # is robot number (1-4)

               192.168.2.101  ...robot 1
               192.168.2.102  ...robot 2
               192.168.2.103  ...robot 3
               192.168.2.104  ...robot 4

  Protocol:    SFTP

  Logon Type:  Normal
  User:        pi
  Pass:        cares

3. Copy one or both of the following to target, where # is robot number:

  launch-robot-#.sh
  robot-client-#.py

4. If updating the launcher script, you may have to make it executable:

  ~ $ chmod 755 launch-robot-1.sh


1.4  Shut down or reboot:

  (Use PuTTY if connecting remoting)

  ~ $ sudo shutdown -h now
  ~ $ sudo shutdown -r now


-------------------------------------------------------------------------------
