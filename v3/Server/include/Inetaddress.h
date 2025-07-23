#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

using std::string;

class Inetaddress{
public:

    Inetaddress(const string &ip, unsigned short port);
    Inetaddress(const struct sockaddr_in &addr);
    ~Inetaddress();

    string ip() const;
    unsigned short port() const;
    const struct sockaddr_in *get_Inetaddrptr() const;
private:
    struct sockaddr_in m_addr;
};