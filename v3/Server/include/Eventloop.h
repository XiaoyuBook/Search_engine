#pragma once
#include "../include/Acceptor.h"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;
using std::mutex;
using std::lock_guard;

class Acceptor;
class Tcp_connection;

using Tcp_connection_ptr = shared_ptr<Tcp_connection>;
using Tcp_connection_callback = function<void(const Tcp_connection_ptr &)>;
using Functor = function<void()>;


class Eventloop{
public:
    Eventloop(Acceptor &acceptor);
    
    ~Eventloop();


    void loop();
    void unloop();

    void wait_epollfd();

    void handle_new_connection();

    void handle_message(int fd);

    void add_epoll_readfd(int fd);

    void del_epoll_readfd(int fd);

    // 注册三个事件
    void set_new_connection_callback(Tcp_connection_callback &&cb);

    void set_message_callback(Tcp_connection_callback &&cb);

    void set_close_callback(Tcp_connection_callback &&cb);

    int create_eventfd();

    int create_epollfd();

    void handle_read();

    void wakeup();

    void do_pending_Functors();

    void run_inloop(Functor &&cb);

private:
    int m_epfd; // epoll_create创建的文件描述符
    vector<struct epoll_event> m_ready_epfd_list;  // 就绪fd的数组
    bool m_islooping;  // 循环标识
    Acceptor & m_acceptor;
    map<int,Tcp_connection_ptr> m_conns; // 存放文件描述符与tcp Connect连接的键值对

    Tcp_connection_callback m_on_new_connection;
    Tcp_connection_callback m_on_message;
    Tcp_connection_callback m_on_close;

    
    int m_eventfd; // 通知用的文件描述符

    vector<Functor> m_pengdings; // 存放任务
    
    mutex m_mutex;

};