#include "../include/file_parse.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

using std::string;
using std::unordered_set;

file_parse::file_parse(const std::vector<string>& file_array,const string & output_file_path)
    : m_file_array(std::move(file_array)),m_output_file_path(output_file_path) {
        // 构造函数每次先把文件清空
        std::ofstream(m_output_file_path, std::ios::trunc).close();
    }

void file_parse::start() {
    for (const auto& path : m_file_array) {
        parse(path);
    }
}

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

void file_parse::parse(const string& path) {
    string content = get_content(path);
    if (content.empty()) return;

    std::vector<string> words;
    m_tokenizer.Cut(content, words);
    output_file(words, m_output_file_path);
}

void file_parse::file_filter(const string& stopwords_path){
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
    while(getline(ifs_s,line)) {
        stopwords.insert(line);
    }
    ifs_s.close();

    // 用来存放临时文件
    std::ofstream temp("temp.txt");
    while(getline(ifs_o, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // 非空且不在筛选列表中
        if(line.find_first_not_of(" \t\n\v\f\r") != std::string::npos && stopwords.find(line) == stopwords.end()) {
            temp << line << "\n";
        }
    }
    ifs_o.close();
    temp.close();
    std::remove(m_output_file_path.c_str());
    std::rename("temp.txt", m_output_file_path.c_str());

}

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