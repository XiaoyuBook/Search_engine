#pragma once

#include "Non_copyable.h"

class Socket : Non_copyable{
public:
    Socket();
    ~Socket();
    explicit Socket(int fd);
    int fd() const;
    void shutdown_write();
private:
    int m_fd;
};