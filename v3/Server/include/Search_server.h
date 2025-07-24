#include "Eventloop.h"
#include "Threadpool.h"
#include "Tcp_server.h"
#include <cstddef>


#include <string>
#include <memory>
#include "Tcp_connection.h"

class MyTask {
    public:
        MyTask(uint16_t type, const std::string& value, const std::shared_ptr<Tcp_connection>& con);
        void process(); 
    
    private:
        uint16_t m_type;
        std::string m_msg;
        std::shared_ptr<Tcp_connection> m_con;
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