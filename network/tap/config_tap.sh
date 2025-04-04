#!/bin/sh

# @desc: when config tap0, ifconfig will show
#
# tap0: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
#       inet 192.168.1.1  netmask 255.255.255.0  broadcast 0.0.0.0
#       ether fe:a4:c0:67:9b:5d  txqueuelen 1000  (Ethernet)
#       RX packets 0  bytes 0 (0.0 B)
#       RX errors 0  dropped 0  overruns 0  frame 0
#       TX packets 0  bytes 0 (0.0 B)
#       TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
#
# Kernel IP routing table
# Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
# default         192.168.3.1     0.0.0.0         UG    30     0        0 eth1
# 192.168.1.0     0.0.0.0         255.255.255.0   U     0      0        0 tap0
# 192.168.3.0     0.0.0.0         255.255.255.0   U     286    0        0 eth1
# 192.168.3.1     0.0.0.0         255.255.255.255 UH    30     0        0 eth1
#       
# 这里要 ping 192.168.1.2, 自己写的监听TAP口的用户态程序才能收到
# 而直接 ping 192.168.1.1  内核会直接处理并回复这个报文, 绑定tap0的IP，内核也会处理
#
config_tap() {
    sudo ip tuntap add dev tap0 mode tap
    sudo ip addr add 192.168.1.1/24 dev tap0
    sudo ip link set tap0 up
    
    echo "[info]: ip link show tap0 is"
    ip link show tap0

    echo "[info]: route"
    route
}

config_tap