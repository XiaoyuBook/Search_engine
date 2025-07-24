#include "../include/Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

Socket::Socket() {
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(m_fd < 0) {
        std::cerr << "socket error" << std::endl; 
        return;
    }
    // 设置非阻塞
    int flags = fcntl(m_fd, F_GETFL, 0);
    if(fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl set nonblock error" << std::endl;
        close(m_fd);
        m_fd = -1;
    }
}

Socket::Socket(int fd): m_fd(fd) {
    int flags = fcntl(m_fd, F_GETFL, 0);
    if(fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl set nonblock error" << std::endl;
        close(m_fd);
        m_fd = -1;
    }
}

Socket::~Socket(){
    close(m_fd);
}

int Socket::fd() const {
    return m_fd;
}

void Socket::shutdown_write(){
    int ret = shutdown(m_fd, SHUT_WR);
    if(ret) {
        std::cerr<< "shutdown" << std::endl;
        return;
    }
}