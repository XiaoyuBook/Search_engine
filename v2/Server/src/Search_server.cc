#include "../include/Search_server.h"
#include "../include/Tcp_connection.h"
#include <cstddef>
#include <iostream>
#include <functional>
#include "../include/web_searcher.h"
#include "../include/candidate_searcher.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

MyTask::MyTask(uint16_t type, const std::string& value, const std::shared_ptr<Tcp_connection>& con)
    : m_type(type), m_msg(value), m_con(con) {}

void MyTask::process() {
    const string web_idx_path = "../../../v1/bin/keywords.txt";
    const string stopwords_path = "../../../v1/stopwords/cn_stopwords.txt";
    const string candidate_idx_path = "../../../v1/bin/index_cn.txt";
    const string dict_path = "../../../v1/bin/dict_cn.txt";
    const string weblib_path = "../../../v1/bin/weblib.txt";
    const string offset_path = "../../../v1/bin/offset.txt";
    const int top_k = 5;

    json response;
    
    if (m_type == 0x0001) { // web_searcher
        web_searcher doc_searcher(
            m_msg, 
            web_idx_path,
            weblib_path,
            offset_path,
            stopwords_path
        );
        // 获取排序后的网页内容
        auto doc_contents = doc_searcher.output(top_k);
        
        // 构建包含网页内容的响应
        json docs_json = json::array();
        for (size_t i = 0; i < doc_contents.size(); ++i) {
            docs_json.push_back({
                {"index", i + 1},
                {"content", doc_contents[i]}
            });
        }
        response["documents"] = docs_json;
        m_con->send_tlv(0x1001, response.dump(2));
    }
    else if (m_type == 0x0002) { // candidate_searcher
        candidate_searcher candidate_searcher(m_msg, candidate_idx_path, dict_path);
        auto candidate_results = candidate_searcher.search(top_k);
        response["candidates"] = candidate_results;
        m_con->send_tlv(0x1002, response.dump(2));
    }
    else {
        m_con->send_tlv(0xFFFF, "Unknown request type.");
    }
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

void Search_server::on_message(const Tcp_connection_ptr &con){
    auto [type, value] = con->receive_tlv(); // 解包TLV
    std::cout << ">> Received TLV. Type: " << type << ", Value: " << value << std::endl;

    MyTask task(type, value, con);
    m_pool.add_task(std::bind(&MyTask::process, task));
}

void Search_server::on_close(const Tcp_connection_ptr &con)
{
    std::cout << con->to_string() << " has close!!!" << std::endl;

}
