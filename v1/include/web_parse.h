#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include "utfcpp/utf8.h"
#include <regex>
#include <iostream>
#include <tinyxml2.h>
#include <unordered_set>


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


class web_parse {
public:
    
    explicit web_parse(const string & offset_filepath, const string & weblib_filepath,const string & keyword_filepath);

    void start();

    void output_offset(std::vector<DocMeta> & docs);

    void output_weblib(std::vector<DocMeta> & docs);
    
    void output_keyword(std::vector<DocMeta> &docs);

    void generate_files_from_list(const std::vector<std::string>& file_list);

    std::vector<DocMeta> generate_unique_docs(const string& file_path); 

    string get_content(const string& path);
    private:
    string m_file_array; // 文件数组
    string m_offset_filepath; // 网页偏移库
    string m_weblib_filepath;  // 网页库
    string m_keyword_filepath; // 关键字
    cppjieba::Jieba m_tokenizer; // 分词器
    simhash::Simhasher m_hasher;  // 去重
};