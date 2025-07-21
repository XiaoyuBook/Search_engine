#include "Eventloop.h"
#include "Threadpool.h"
#include "Tcp_server.h"
#include <cstddef>


class MyTask {
public:
    MyTask(const string & msg, const Tcp_connection_ptr &con);
    void process();

private:
    string m_msg;
    Tcp_connection_ptr m_con;
};

class Search_server{
public:
    Search_server(size_t thread_num, size_t queue_size, const string &ip, unsigned short port);
    ~Search_server();

    void start();
    void stop();

    void on_new_connection(const Tcp_connection_ptr &con);
    void on_message(const Tcp_connection_ptr &con);
    void on_close(const Tcp_connection_ptr &con);

private:
    Thread_pool m_pool;
    Tcp_server m_server;
};