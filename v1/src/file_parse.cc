#include "../include/file_parse.h"
#include <cctype>
#include <cstdio>
#include <fstream>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <map>
#include <string>
#include <ctype.h>

#include "utfcpp/utf8.h"

#define TYPE_CN 1
#define TYPE_EN 2

using std::string;
using std::unordered_set;
using std::map;




file_parse::file_parse(const std::vector<string>& file_array,const string & output_file_path)
    : m_file_array(std::move(file_array)),m_output_file_path(output_file_path) {
        // 构造函数每次先把文件清空
        std::ofstream(m_output_file_path, std::ios::trunc).close();
    }


// 开始解析：从文件夹中获取到所有文件名并且放入到一个数组中
void file_parse::start(const int & flag) {
    for (const auto& path : m_file_array) {
        if(flag == TYPE_CN)cn_parse(path);
        else if(flag == TYPE_EN)en_parse(path);
    }
}

// 获取文件内容（中英通用）
string file_parse::get_content(const string& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Error: Failed to open " << path << std::endl;
        return "";
    }
    
    std::stringstream buf;
    buf << ifs.rdbuf();
    return buf.str();
}

// 中文分词
void file_parse::cn_parse(const string& path) {
    string content = get_content(path);
    if (content.empty()) return;

    std::vector<string> words;
    m_tokenizer.Cut(content, words);
    output_file(words, m_output_file_path);
}

bool contains_digit(const string &s){
    for(char e : s) {
        if(isdigit(static_cast<unsigned char>(e))) {
            return true;
        }
    }
    return false;
}
// 英文分词
void file_parse::en_parse(const string &path) {
    string content = get_content(path);
    if(content.empty()) return;
    
    std::vector<string> words;
    string word;
    std::istringstream iss{content};
    while(iss >> word) {
        words.push_back(word);
    }
    
    output_file(words, m_output_file_path);
}


// 去除空格
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// 检查是否有英文或者数字
bool contains_alpha_or_digit(const std::string& str) {
    for (char c : str) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

// 过滤停用词
void file_parse::file_filter(const string& stopwords_path, const int & flag) {
    std::ifstream ifs_o(m_output_file_path);
    std::ifstream ifs_s(stopwords_path);
    if (!ifs_o) {
        std::cerr << "Error opening " << m_output_file_path << ": ";
        perror("");
        return;
    }
    if (!ifs_s) {
        std::cerr << "Error opening " << stopwords_path << ": ";
        perror("");
        return;
    }
  
    unordered_set<string> stopwords;
    string line;
    while (getline(ifs_s, line)) {
        stopwords.insert(trim(line));
    }
    ifs_s.close();
    std::ofstream temp("temp.txt");

    // 中文
    if(flag == TYPE_CN){

        while (getline(ifs_o, line)) {
            string clean_line = trim(line);
            if(clean_line.empty()) continue;
            if (stopwords.find(clean_line) == stopwords.end() && !contains_alpha_or_digit(clean_line)) {
                temp << clean_line << "\n";
            }
        }

    }

    // 中文结束

     // 英文
    if(flag == TYPE_EN) {

        while (getline(ifs_o, line)) {
            string clean_line = trim(line);
            if(clean_line.empty()) continue;
            if(contains_digit(line)) continue;
            if (stopwords.find(clean_line) == stopwords.end()) {
                temp << clean_line << "\n";
            }
        }

    } 
    // 英文结束

    ifs_o.close();
    temp.close();

    std::remove(m_output_file_path.c_str());
    std::rename("temp.txt", m_output_file_path.c_str());
}




// 生成输出文件（通中英通用）
void file_parse::output_file(const std::vector<string>& words,const string& filename) {
    std::ofstream outfile(filename, std::ios::app);
    if (!outfile) {
        std::cerr << "Error: Failed to open output file " << filename << std::endl;
        return;
    }
    
    for (const auto& word : words) {
        outfile << word << "\n";
    }
    outfile.close();
}





// 统计频率 (中英通用)
void file_parse::count_words(const string & path){
    std::ifstream ifs(m_output_file_path);
    if(!ifs.is_open()) {
        std::cerr << "while counting: open output.txt failed" <<std::endl;
        return;
    }
    map<string, int> count;
    string line;
    while(getline(ifs, line)) {
        count[line]++;
    }
    ifs.close();

    // 生成相应的频率文件
    std::ofstream count_file(path);
    for(auto &[key,value] : count) {
        count_file << key << " " << value << "\n";
    }
}
