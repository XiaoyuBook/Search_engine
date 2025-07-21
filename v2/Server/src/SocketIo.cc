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
    char *pstr = buf;
    int ret = 0;

    while(left > 0) {
        ret = read(m_fd, pstr, left);
        if(ret == -1 && errno == EINTR) {
            continue;
        } else if (ret == -1) {
            std::cerr << " read error" << std::endl;
            return -1;
        } else if(ret == 0) {
            break;
        } else {
            pstr += ret;
            left -= ret;
        }
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
            } else if(errno == EAGAIN) { // 非阻塞下无数据，退出等待
                break;
            } else {
                std::cerr << "readline error: " << strerror(errno) << std::endl;
                return -1;
            }
        } else if(ret == 0) {
            break; // 连接关闭
        } else {
            // 查找换行符（现有逻辑保留）
            for(int idx = 0; idx < ret; idx++) {
                if(pstr[idx] == '\n') {
                    int sz = idx +1;
                    readn(pstr, sz);
                    pstr += sz;
                    *pstr = '\0';
                    return total + sz;
                }
            }
            // 未找到换行符，读取所有数据
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