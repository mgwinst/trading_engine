#!/bin/bash

SUB_INTERFACE="eno1.131"
IP_ADDR="10.0.131.35/24"
MULTICAST_ADDR="01:00:5E:7F:00:01" # 239.255.0.1

sudo ip link set dev $SUB_INTERFACE down || {
    "Failed to bring down $SUB_INTERFACE"
    exit 1
}

sudo ip addr del $IP_ADDR dev $SUB_INTERFACE || {
    "Failed to delete IP address $IP_ADDR on $SUB_INTERFACE"
    exit 1
}

sudo ip maddr del $MULTICAST_ADDR dev $SUB_INTERFACE || {
    "Failed to leave multicast group $MULTICAST_ADDR on $SUB_INTERFACE"
    exit 1
}

sudo ip link del $SUB_INTERFACE || {
    "Failed to delete sub interface $SUB_INTERFACE"
    exit 1
}

echo "Successful multicast unsubscription and VLAN sub-interface deletion"
