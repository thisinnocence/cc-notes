#!/bin/sh

# after config tun0, `ifconfig tun0` show:
# tun0: flags=4241<UP,POINTOPOINT,NOARP,MULTICAST>  mtu 1500
#       inet 192.168.1.1  netmask 255.255.255.0  destination 192.168.1.1
#       unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 500  (UNSPEC)
# 
# 注：要ping 192.168.1.1 还是会被内核协议栈处理，并回复ping报文
# 
config_tun() {
    sudo ip tuntap add dev tun0 mode tun
    sudo ip addr add 192.168.1.1/24 dev tun0
    sudo ip link set dev tun0 up

    echo "[info]: ip link show tun0 is"
    ip link show tun0

    echo "[info]: route"
    route
}

config_tun