#pragma once

#include "Acceptor.h"
#include "Eventloop.h"

class Tcp_server {
public:
    Tcp_server(const string &ip, unsigned short port);
    ~Tcp_server();

    void start();
    void stop();

    void set_all_callback(
        Tcp_connection_callback &&cb1,
        Tcp_connection_callback &&cb2,
        Tcp_connection_callback &&cb3
    );
private:
    Acceptor m_acceptor;
    Eventloop m_loop;
};