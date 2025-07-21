#include "../include/Search_server.h"
#include "../include/Tcp_connection.h"
#include <cstddef>
#include <iostream>
#include <functional>

MyTask::MyTask(const string &msg, const Tcp_connection_ptr &con)
:m_msg(msg),
m_con(con){

}

void MyTask::process() {
    m_con->send_inloop(m_msg);
}


Search_server::Search_server(size_t thread_num, size_t queue_size, const string& ip, unsigned short port):
m_pool(thread_num,queue_size),
m_server(ip,port){

}


Search_server::~Search_server(){}


void Search_server::start()
{
    m_pool.start();
    //function<void(const TcpConnectionPtr &)>
    using namespace std::placeholders;
    m_server.set_all_callback(std::bind(&Search_server::on_new_connection, this, _1)
                           , std::bind(&Search_server::on_message, this, _1)
                           , std::bind(&Search_server::on_close, this, _1));
    m_server.start();

}
void Search_server::stop()
{
    m_pool.stop();
    m_server.stop();
}

//三个回调
void Search_server::on_new_connection(const Tcp_connection_ptr &con)
{
    std::cout << con->to_string() << " has connected!!!" << std::endl;
}

void Search_server::on_message(const Tcp_connection_ptr &con)
{
    string msg = con->receive();
    std::cout << ">>recv msg from client: " << msg << std::endl;

    MyTask task(msg, con);
    m_pool.add_task(std::bind(&MyTask::process, task));
}

void Search_server::on_close(const Tcp_connection_ptr &con)
{
    std::cout << con->to_string() << " has close!!!" << std::endl;

}
