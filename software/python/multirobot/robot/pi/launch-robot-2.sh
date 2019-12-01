#!/bin/sh
# launch-robot-2.sh
echo `date +"%Y-%m-%d %H:%M:%S"` `hostname`
#sleep 20
cd /
cd /home/pi
echo `date +"%Y-%m-%d %H:%M:%S"` robot-client-2.py
sudo python3 robot-client-2.py $1
sudo shutdown -h now
