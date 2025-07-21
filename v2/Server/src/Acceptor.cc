#include "../include/Acceptor.h"
#include <asm-generic/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

Acceptor::Acceptor(const string &ip, unsigned short port):m_sock(), m_addr(ip,port) {

}

Acceptor::~Acceptor(){}

void Acceptor::ready() {
    set_reuse_addr();
    set_reuse_port();
    bind();
    listen();
}

void Acceptor::set_reuse_addr() {
    int flag = 1;
    int ret = setsockopt(m_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(ret == -1) {
        std::cerr << "setsockopt" << std::endl;
        return;
    }
}

void Acceptor::set_reuse_port() {
    int flag = 1;
    int ret = setsockopt(m_sock.fd(), SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
    if(ret == -1) {
        std::cerr << "setsockopt" << std::endl;
        return;
    }
}

void Acceptor::bind() {
    int ret = ::bind(m_sock.fd(), (struct sockaddr *)m_addr.get_Inetaddrptr(), sizeof(struct sockaddr));
    if(ret == -1) {
        std::cerr << "bind" << std::endl;
        return;
    }
}

void Acceptor::listen() {
    int ret = ::listen(m_sock.fd(), 128);
    if(ret == -1) {
        std::cerr << "listen " << std::endl;
        return;
    }
}

int Acceptor::accept() {
    int fd = ::accept(m_sock.fd(), nullptr, nullptr);
    if(fd == -1) {
        std::cerr << "accept" << std::endl;
        return -1;
    }
    return fd;
}

int Acceptor::fd() const {
    return m_sock.fd();
}