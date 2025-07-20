#pragma once

#include <vector>
#include <string>
#include "cppjieba/Jieba.hpp"
#include "simhash/Simhasher.hpp"
using std::string;

struct DocMeta {
    std::string id;
    std::string link;
    std::string title;
    std::string content;
    size_t size;
};

std::string trim(const std::string& s);

string contains_alpha(const string &s);
bool contains_non_chinese(const std::string& str);
class file_parse {

public:

    // 构造函数 第一个参数是分好词的生成文件，第二个文件是doc文件，第三个文件
    explicit file_parse(
        const std::vector<std::string>& file_array, 
        const string & output_file_path, 
        const string & offset_file_path,
        const string & weblib_path);
    
    // 默认析构函数
    ~file_parse() = default;
    
    // 开始解析所有文件
    void start(const int &flag);
    // 删除停用词以及空格
    void file_filter(const string& path, const int &flag);
    // 统计频率
    void count_words(const string& path);
    // 生成索引
    void create_index(const string & intput_path, const string & output_path);

    std::vector<DocMeta> generate_unique_docs(const string& file_path);

    
private:
    
    void output_offset(std::vector<DocMeta> & docs);

    void output_weblib(std::vector<DocMeta> & docs);
    
    void output_keyword(std::vector<DocMeta> &docs);

    // 获取文件内容
    std::string get_content(const std::string& path);
    
    // 对单个文件进行分词处理
    void cn_parse(const std::string& path);
    void en_parse(const string &path);


    // 将分词结果写入输出文件
    void output_file(const std::vector<std::string>& words, 
                   const std::string& filename);
    
    std::vector<std::string> m_file_array;
    cppjieba::Jieba m_tokenizer;
    simhash::Simhasher m_hasher;
    string m_output_file_path;  // 文档的关键字集合
    string m_offset_file_path; // 网页偏移库
    string m_weblib_path;  // 网页库
};

