#!/bin/bash
set -e

PI_USER=scylla
PI_IP=scylla.local

# convert received TCP back to UDP
socat -u TCP-LISTEN:7667,reuseaddr,fork UDP-SENDTO:239.255.76.67:7667 &
SOCAT_PID=$!
echo "[local] socat PID: $SOCAT_PID"

MY_IP=$(ifconfig | grep 'inet 192' | awk '{print $2}')

# ssh into rpi and convert UDP to TCP
ssh $PI_USER@$PI_IP "nohup socat -u UDP-RECV:7667 TCP:${MY_IP}:7667 > /dev/null 2>&1 &"
# ssh $PI_USER@$PI_IP "nohup socat UDP-RECVFROM:7667,fork TCP:${MY_IP}:7667 > /dev/null 2>&1 &"

echo "Connected to RPI"
echo "Press CTRL-C to close"

trap "echo 'Cleaning up...'; kill $SOCAT_PID; sleep 2; ssh $PI_USER@$PI_IP 'killall socat'" EXIT

wait
