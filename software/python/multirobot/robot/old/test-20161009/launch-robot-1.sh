#!/bin/sh
# launch-robot-1.sh
echo `date +"%Y-%m-%d %H:%M:%S"` `hostname`
#sleep 20
cd /
cd /home/lucas
echo `date +"%Y-%m-%d %H:%M:%S"` robot-client-1.py
sudo python3 robot-client-1.py $1
#sudo shutdown -h now
