#include "../include/Eventloop.h"
#include "../include/Acceptor.h"
#include "../include/Tcp_connection.h"
#include <asm-generic/errno-base.h>
#include <cstdint>
#include <mutex>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <iostream>
#include <vector>


using std::cerr;
using std::cout;
using std::endl;



Eventloop::Eventloop(Acceptor & acceptor):
m_epfd(create_epollfd()),
m_ready_epfd_list(1024),
m_islooping(false),
m_acceptor(acceptor),
m_eventfd(create_eventfd()),
m_pengdings(),
m_mutex(){
    int listen_fd = m_acceptor.fd();
    add_epoll_readfd(listen_fd);
    add_epoll_readfd(m_eventfd);
}


Eventloop::~Eventloop() {
    close(m_epfd);
    close(m_eventfd);
}


void Eventloop::loop() {
    // 开始循环
    m_islooping = true;

    while(m_islooping) {
        wait_epollfd();
    }
}



void Eventloop::unloop() {
    m_islooping = false;
}


void Eventloop::wait_epollfd() {
    int nready = 0;
    do{
        nready = epoll_wait(m_epfd, m_ready_epfd_list.data(), m_ready_epfd_list.size(), 300);
    }while( nready == 01 && errno == EINTR );



    if(nready == -1) {
        cerr << "nready == -1" << endl;
        return;
    } else if(nready == 0) {
        cout << " epoll_wait timeout!" << endl;
    } else {
        // 如果满载
        if((int)m_ready_epfd_list.size() == nready) {
            m_ready_epfd_list.resize(2 * nready);
        }

        for(int idx = 0; idx < nready; idx++) {
            int listen_fd = m_acceptor.fd();
            int fd = m_ready_epfd_list[idx].data.fd;


            if(fd == listen_fd) {// 新连接
                handle_new_connection();  // 回调新连接函数
            } else if(fd == m_eventfd) { // 处理线程间通信
                    handle_read(); // 读取任务
                    
                    do_pending_Functors();

            } else {
                handle_message(fd); // 处理客户端通信
            }           
        }
    }
}


void Eventloop::handle_new_connection() {
    int connfd = m_acceptor.accept();
    if(connfd < 0) {
        cerr << "handle_new_connection" << endl;
        return;
    }

    add_epoll_readfd(connfd); // 监听
    // 转换为Connect连接对象
    Tcp_connection_ptr con(new Tcp_connection(connfd, this));

    con->set_new_connection_callback(m_on_new_connection);
    con->set_message_callback(m_on_message);
    con->set_close_callback(m_on_close);
    
    m_conns[connfd] = con;

    con->handle_new_connection_callback();
}

void Eventloop::handle_message(int fd)  {
    auto it = m_conns.find(fd);
    if(it != m_conns.end()) {
        bool flag = it->second->isclosed();
        if(flag) {
            it->second->handle_close_callback();
            del_epoll_readfd(fd);
            m_conns.erase(it);
        } else {
            it->second->handle_message_callback();
        }
    } else {
        cout << "连接不存在" <<endl;
        return;
    }
}

int Eventloop::create_epollfd() {
    int fd = epoll_create(10);
    if(fd < 0) {
        cerr << "create_epollfd" << endl;
        return -1;
    }
    return fd;
}

void Eventloop::add_epoll_readfd(int fd) {
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;
    int ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0) {
        std::cerr<< "add_epoll_readfd" << endl;
        return;
    }
}


void Eventloop::del_epoll_readfd(int fd) {
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;
    
    int ret = epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &evt);

    if(ret < 0) {
        cerr << "del_epoll_read" << endl;
        return;
    }
}



void Eventloop::set_new_connection_callback(Tcp_connection_callback &&cb) {
    m_on_new_connection = std::move(cb);
}

void Eventloop::set_message_callback(Tcp_connection_callback &&cb) {
    m_on_message = std::move(cb);
}

void Eventloop::set_close_callback(Tcp_connection_callback &&cb) {
    m_on_close = std::move(cb);
}


int Eventloop::create_eventfd() {
    int fd = eventfd(0,0);
    if(fd < 0) {
        cerr << "create_event_fd"<< endl;
        return -1;
    }
    return fd;

}

void Eventloop::handle_read() {
    uint64_t one =1;
    ssize_t ret = read(m_eventfd, &one, sizeof(uint64_t));
    if(ret!= sizeof(uint64_t)) {
        cerr<< "handle_read" <<endl;
    }
}

void Eventloop::wakeup() {
    uint64_t one = 1;
    ssize_t ret = write(m_eventfd, &one, sizeof(uint64_t));

    if(ret != sizeof(uint64_t)){
        cerr<< "wakeup" <<endl;
    }
}

void Eventloop::do_pending_Functors() {
    vector<Functor> tmp;
    {
        lock_guard<mutex> lg(m_mutex);
        tmp.swap(m_pengdings);
    }

    for(auto &cb: tmp) {
        cb();
    }
}




void Eventloop::run_inloop(Functor &&cb) {
    {
        lock_guard<mutex> lg(m_mutex);
        m_pengdings.push_back(std::move(cb));
    }
    wakeup();
}