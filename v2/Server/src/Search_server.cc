#include "../include/Search_server.h"
#include "../include/Tcp_connection.h"
#include <cstddef>
#include <iostream>
#include <functional>
#include "../include/web_searcher.h"
#include "../include/candidate_searcher.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

MyTask::MyTask(const std::string& msg, const std::shared_ptr<Tcp_connection>& con)
    : m_msg(msg), m_con(con)  // 初始化列表必须正确赋值成员变量
{
    // 可选：构造时的初始化逻辑
}

// 任务处理：执行搜索并返回结果
void MyTask::process() {
    // 1. 配置文件路径（根据实际项目结构调整，确保能找到索引/字典/停用词文件）
    const std::string web_idx_path = "../../../v1/bin/keywords.txt";       // web_searcher的索引文件
    const std::string stopwords_path = "../../../v1/stopwords/cn_stopwords.txt"; // 停用词文件
    const std::string candidate_idx_path = "../../../v1/bin/index_cn.txt"; // 候选词索引
    const std::string dict_path = "../../../v1/bin/dict_cn.txt";           // 候选词字典
    const int top_k = 5; // 返回前5条结果

    // 2. 网页搜索：使用web_searcher获取相关文档
    web_searcher doc_searcher(m_msg, web_idx_path);
    auto doc_results = doc_searcher.search_topk(top_k, stopwords_path);

    // 3. 候选词推荐：使用candidate_searcher获取候选词
    candidate_searcher candidate_searcher(m_msg, candidate_idx_path, dict_path);
    auto candidate_results = candidate_searcher.search(top_k);

    // 4. 构造返回的JSON结果
    json response;
    // 整理文档搜索结果
    json docs_json = json::array();
    for (const auto& [docid, score] : doc_results) {
        docs_json.push_back({{"docid", docid}, {"similarity", score}});
    }
    response["documents"] = docs_json;
    // 整理候选词结果（candidate_searcher已返回json数组）
    response["candidates"] = candidate_results;

    // 5. 将JSON结果发送给客户端
    std::string response_str = response.dump(2); // 格式化JSON，便于阅读
    m_con->send_inloop(response_str);
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
