#pragma once
#include "Socket.h"
#include "Inetaddress.h"

using std::string;

class Acceptor{
public:
    Acceptor(const string &ip, unsigned short port);
    ~Acceptor();
    void ready();
    int accept();
    int fd() const;

private:
    void set_reuse_addr();
    void set_reuse_port();
    void bind();
    void listen();

    Socket m_sock;
    Inetaddress m_addr;
};