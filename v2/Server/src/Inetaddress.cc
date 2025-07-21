#include "../include/Inetaddress.h"
#include <netinet/in.h>
#include <strings.h>


Inetaddress::Inetaddress(const string & ip, unsigned short port) {
    bzero(&m_addr, sizeof(sockaddr_in));

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());

}

Inetaddress::Inetaddress(const struct sockaddr_in &addr): m_addr(addr) {

}

Inetaddress::~Inetaddress(){

}

string Inetaddress::ip() const {
    return string(inet_ntoa(m_addr.sin_addr));
}

unsigned short Inetaddress::port() const{
    return ntohs(m_addr.sin_port);
}

const struct sockaddr_in *Inetaddress::get_Inetaddrptr() const{
    return &m_addr;
}