#pragma once 

#include "../include/Socket.h"
#include "../include/SocketIo.h"
#include "../include/Inetaddress.h"
#include "Eventloop.h"

#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;


class Eventloop;

class Tcp_connection:public std::enable_shared_from_this<Tcp_connection>{
    using Tcp_connection_ptr = shared_ptr<Tcp_connection>;
    using Tcp_connection_callback = function<void(const Tcp_connection_ptr &)>;


public:
    // tlv新加
    void send_tlv(uint16_t type, const std::string& value);
    std::pair<uint16_t, std::string> receive_tlv();
    // tlv
    explicit Tcp_connection(int fd, Eventloop *loop);
    ~Tcp_connection();
    void send(const string &msg);
    void send_inloop(const string &msg);
    string receive();
    bool isclosed() const;

    string to_string();

    void set_new_connection_callback(const Tcp_connection_callback &cb);
    void set_message_callback(const Tcp_connection_callback &cb);
    void set_close_callback(const Tcp_connection_callback &cb);


    void handle_new_connection_callback();
    void handle_message_callback();
    void handle_close_callback();


private:
    Inetaddress get_loacl_addr();
    Inetaddress get_peer_addr();

    Eventloop *m_loop;
    SocketIo m_sockIo;

    Socket m_sock;
    Inetaddress m_local_addr;
    Inetaddress m_peer_addr;

    Tcp_connection_callback m_on_new_connection;
    Tcp_connection_callback m_on_message;
    Tcp_connection_callback m_on_close;
};