
Client software installed on Debian Linux to emulate robots

Copy the same Python scripts as installed on the robots:
  robot-client-1.py
  robot-client-2.py
  robot-client-3.py
  robot-client-4.py

Used the modified launch scripts, which unlike the robot
versions do not shutdown after the Python script returns:
  launch-robot-1.sh
  launch-robot-2.sh
  launch-robot-3.sh
  launch-robot-4.sh


1.  Edit Python scripts to set debug flag.

2.  Open a terminal instance for each robot.

3.  Launch script:

  ~ $ sudo ./launch-robot-#.sh  ...where # is robot number

4.  Stop script by issuing robot shutdown command from the
    server software, or at the terminal by pressing:

  ctrl + shift + \
