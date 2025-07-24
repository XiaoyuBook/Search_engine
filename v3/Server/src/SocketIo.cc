#include "../include/SocketIo.h"
#include <asm-generic/errno-base.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

SocketIo::SocketIo(int fd):m_fd(fd){

}

SocketIo::~SocketIo(){

}


int SocketIo::readn(char *buf, int len) {
    int left = len;
    char *p = buf;
    while (left > 0) {
        ssize_t ret = ::read(m_fd, p, left);
        if (ret == -1) {
            if (errno == EINTR) {  
                continue;
            } else { 
                std::cerr << "read error: " << strerror(errno) << std::endl;
                return -1;
            }
        } else if (ret == 0) { 
            break;
        }
        left -= ret;
        p += ret;
    }
    return len - left;  
}

int SocketIo::readline(char *buf, int len) {
    int left = len -1;
    char *pstr = buf;
    int ret = 0, total = 0;

    while(left > 0) {
        ret = recv(m_fd, pstr, left, MSG_PEEK);
        if(ret == -1) {
            if(errno == EINTR) {
                continue;
            } else if(errno == EAGAIN) {
                break;
            } else {
                std::cerr << "readline error: " << strerror(errno) << std::endl;
                return -1;
            }
        } else if(ret == 0) {
            break; 
        } else {
            for(int idx = 0; idx < ret; idx++) {
                if(pstr[idx] == '\n') {
                    int sz = idx +1;
                    readn(pstr, sz);
                    pstr += sz;
                    *pstr = '\0';
                    return total + sz;
                }
            }
            readn(pstr, ret);
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }
    *pstr = '\0';
    return total;
}

int SocketIo::writen(const char *buf, int len) {
    int left = len;
    const char *pstr = buf;
    int ret = 0;

    while(left > 0) {
        ret = write(m_fd, pstr, left);
        if(ret == -1 && errno == EINTR) {
            continue;
        } else if (ret == -1) {
            std::cerr << "writen error" << std::endl;
            break;
        } else if(ret == 0) {
            break;
        }else {
            pstr += ret;
            left -= ret;
        }
    }
    return len - left;
}