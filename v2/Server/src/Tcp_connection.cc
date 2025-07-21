#include "../include/Tcp_connection.h"
#include "../include/Eventloop.h"
#include <iostream>
#include <sstream>
#include <sys/socket.h>

using std::cout;
using std::endl;
using std::ostringstream;


Tcp_connection::Tcp_connection(int fd, Eventloop *loop):
m_loop(loop),
m_sockIo(fd),
m_sock(fd),
m_local_addr(get_loacl_addr()),
m_peer_addr(get_peer_addr()) {

}

Tcp_connection::~Tcp_connection() {

}

void Tcp_connection::send(const string & msg) {
    m_sockIo.writen(msg.c_str(), msg.size());
}


void Tcp_connection::send_inloop(const string &msg) {
    if(m_loop) {
        m_loop->run_inloop(bind(& Tcp_connection::send, this, msg));
    }
}

string Tcp_connection::receive() {
    char buf[65535] = {0};
    m_sockIo.readline(buf, sizeof(buf));

    return string(buf);
}



bool Tcp_connection::isclosed() const {
    char buf[10] = {0};
    int ret = recv(m_sock.fd(), buf, sizeof(buf), MSG_PEEK  );

    return ret == 0;
}



string Tcp_connection::to_string(){
    ostringstream oss;
    oss << m_local_addr.ip()<< ":" << m_local_addr.port() << "--->"
    << m_peer_addr.ip() << ": " << m_peer_addr.port();


    return oss.str();
}

Inetaddress Tcp_connection::get_loacl_addr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    int ret = getsockname(m_sock.fd(), (struct sockaddr*)&addr, &len);
    if(ret == -1) {
        std::cerr<< "getsockname" << std::endl;
    }
    return Inetaddress(addr);
}



Inetaddress Tcp_connection::get_peer_addr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    int ret = getpeername(m_sock.fd(), (struct sockaddr*)&addr, &len);
    if(ret == -1) {
        std::cerr<< "getpeername" << std::endl;
    }
    return Inetaddress(addr);
}

// 注册回调函数
void Tcp_connection::set_new_connection_callback(const Tcp_connection_callback &cb){
    m_on_new_connection = cb;
};

void Tcp_connection::set_message_callback(const Tcp_connection_callback &cb) {
    m_on_message = cb;
}

void Tcp_connection::set_close_callback(const Tcp_connection_callback &cb) {
    m_on_close = cb;
}

// 回调函数的执行
void Tcp_connection::handle_new_connection_callback() {
    if(m_on_new_connection) {
        m_on_new_connection(shared_from_this());
    } else {
        cout << "m_on_new_connection == nullptr" << endl;

    }
}


void Tcp_connection::handle_message_callback() {
    if(m_on_message) {
        m_on_message(shared_from_this());
    } else {
        cout << "m_on_message == nullptr" <<endl;
    }
}

void Tcp_connection::handle_close_callback() {
    if(m_on_close) {
        m_on_close(shared_from_this());
    } else {
        cout << "m_on_close == nullptr" <<endl;
    }
}