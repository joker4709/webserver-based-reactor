#include "inet_address.h"
#include <arpa/inet.h>
#include <netinet/in.h>

InetAddress::InetAddress(const string &ip, const unsigned short port) 
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in &addr):
    addr_(addr)
{

}

string InetAddress::getIP() 
{
    return inet_ntoa(addr_.sin_addr);
}

unsigned short InetAddress::getPort() 
{
    return ntohs(addr_.sin_port);
}

InetAddress::~InetAddress() 
{

}

struct sockaddr_in *InetAddress::getInetAddressPtr()
{
    return &addr_;
}