#!/bin/sh
# launch-robot.sh
echo `date +"%Y-%m-%d %H:%M:%S"` `hostname`
sleep 20
cd /
cd /home/pi
echo `date +"%Y-%m-%d %H:%M:%S"` robot_client.py
sudo python3 robot_client.py
cd /
