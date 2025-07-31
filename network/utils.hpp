#pragma once

#include <netdb.h>
#include <ifaddrs.h>
#include <string>

namespace network::utilities
{
    inline std::string get_iface_ip(const std::string_view interface) 
    {
        char buffer[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr = nullptr;
        
        if (getifaddrs(&ifaddr) != -1) {
            ifaddrs* ifa = ifaddr;
            while (ifa) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && interface == ifa->ifa_name) {
                    getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);
                    break;
                }
                ifa = ifa->ifa_next;
            }
        }

        freeifaddrs(ifaddr);
        return std::string{buffer};
    }






}
