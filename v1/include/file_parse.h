#pragma once

#include <vector>
#include <string>
#include "cppjieba/Jieba.hpp"

class file_parse {
public:
    // 构造函数（使用移动语义优化）
    explicit file_parse(const std::vector<std::string>& file_array);
    
    // 默认析构函数
    ~file_parse() = default;
    
    // 开始解析所有文件
    void start();

private:
    // 获取文件内容
    std::string get_content(const std::string& path);
    
    // 对单个文件进行分词处理
    void parse(const std::string& path);
    
    // 将分词结果写入输出文件
    void output_file(const std::vector<std::string>& words, 
                   const std::string& filename);
    
    std::vector<std::string> m_file_array;
    cppjieba::Jieba m_tokenizer;
};

