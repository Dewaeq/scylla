#!/bin/bash
set -e

LCM_IP=239.255.76.67
LCM_PORT=7667
TUNNEL_PORT=7666
PI_USER=scylla
PI_IP=scylla.local

# convert received TCP to local UDP (pi -> pc)
socat -u TCP-LISTEN:$TUNNEL_PORT,reuseaddr,fork UDP-SENDTO:239.255.76.67:$LCM_PORT &

MY_IP=$(ifconfig | grep 'inet 192' | awk '{print $2}')

# ssh into rpi and convert local UDP to TCP (pi -> pc)
ssh $PI_USER@$PI_IP "nohup socat -u UDP-RECV:${LCM_PORT},ip-add-membership=239.255.76.67:0.0.0.0,reuseaddr TCP:${MY_IP}:${TUNNEL_PORT} > /dev/null 2>&1 &"

# ssh into rpi and convert TCP to local UDP (pc -> pi)
ssh $PI_USER@$PI_IP "nohup socat -u TCP-LISTEN:${TUNNEL_PORT},reuseaddr,fork UDP-SENDTO:239.255.76.68:$LCM_PORT > /dev/null 2>&1 &"

# convert local UDP to TCP for rpi (pc -> pi)
socat -u UDP-RECV:$LCM_PORT,ip-add-membership=239.255.76.68:0.0.0.0,reuseaddr TCP:$PI_IP:$TUNNEL_PORT &

echo "Connected to RPI"
echo "Press CTRL-C to close"

trap "echo 'Cleaning up...'; ssh $PI_USER@$PI_IP 'killall socat'; killall socat" EXIT

wait
