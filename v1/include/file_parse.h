#pragma once

#include <vector>
#include <string>
#include "cppjieba/Jieba.hpp"

using std::string;

std::string trim(const std::string& s);

string contains_alpha(const string &s);

bool contains_non_chinese(const std::string& str);
class file_parse {

public:

    // 构造函数
    explicit file_parse(const std::vector<std::string>& file_array, const string & output_file_path);

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

private:


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
    string m_output_file_path;
};