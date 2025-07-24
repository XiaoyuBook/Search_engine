#include "../include/Tcp_server.h"

Tcp_server::Tcp_server(const string &ip, unsigned short port):
m_acceptor(ip,port),
m_loop(m_acceptor) {

}


Tcp_server::~Tcp_server() {

}


void Tcp_server::start() {
    m_acceptor.ready();
    m_loop.loop();
}



void Tcp_server::stop(){
    m_loop.unloop();
}


void Tcp_server::set_all_callback(Tcp_connection_callback &&cb1, Tcp_connection_callback &&cb2, Tcp_connection_callback &&cb3) {
    m_loop.set_new_connection_callback(std::move(cb1));
    m_loop.set_message_callback(std::move(cb2));
    m_loop.set_close_callback(std::move(cb3));
}
