#ifndef INET_ADDRESS_H__
#define INET_ADDRESS_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

using std::string;

class InetAddress 
{
public:
    InetAddress(const string &ip, const unsigned short port);
    InetAddress(const struct sockaddr_in &addr);

    string getIP();
    unsigned short getPort();

    struct sockaddr_in *getInetAddressPtr();
    ~InetAddress();
private:
    struct sockaddr_in addr_;
};


#endif