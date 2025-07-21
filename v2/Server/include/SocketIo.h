#pragma once

class SocketIo{
public:
    explicit SocketIo(int fd);
    ~SocketIo();

    int readn(char *buf, int len);
    int readline(char *buf, int len);
    int writen(const char *buf, int len);
private:
    int m_fd;
};