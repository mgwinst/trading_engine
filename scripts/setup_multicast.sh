#!/bin/bash

# configure VLAN sub-interface and mutlicast subscription

INTERFACE="eno1"
VLAN_ID="131"
SUB_INTERFACE="${INTERFACE}.${VLAN_ID}"
IP_ADDR="10.0.131.35/24"
MULTICAST_MAC_ADDR="01:00:5E:7F:00:01" # 239.255.0.1
MULTICAST_IP_ADDR="239.255.0.1"

if ip link show $SUB_INTERFACE &>/dev/null; then
    echo "Sub-interface $SUB_INTERFACE already exists"
    exit 0
fi

sudo ip link add link $INTERFACE name $SUB_INTERFACE type vlan id $VLAN_ID || {
    echo "Failed to create sub-interface $SUB_INTERFACE"
    exit 1
}

sudo ip link set $SUB_INTERFACE up || {
    echo "Failed to bring up $SUB_INTERFACE"
    exit 1
}

sudo ip addr add $IP_ADDR dev $SUB_INTERFACE || {
    echo "Failed to assign IP address $IP_ADDR to $SUB_INTERFACE"
    exit 1
}

sudo ip maddr add $MULTICAST_ADDR dev $SUB_INTERFACE || {
    echo "Failed to join $MULTICAST_ADDR on $SUB_INTERFACE"
    exit 1
}

sudo smcroute -j $SUB_INTERFACE 239.255.0.1 || {
    echo "smcroute failed"
    exit 1
}
