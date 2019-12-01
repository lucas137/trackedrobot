#!/bin/sh
# launch-robot-4.sh
echo `date +"%Y-%m-%d %H:%M:%S"` `hostname`
#sleep 20
cd /
cd /home/lucas
echo `date +"%Y-%m-%d %H:%M:%S"` robot-client-4.py
sudo python3 robot-client-4.py $1
#sudo shutdown -h now
