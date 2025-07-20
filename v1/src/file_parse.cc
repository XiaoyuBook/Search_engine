#include "../include/file_parse.h"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <istream>
#include <iterator>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <map>
#include <string>
#include <ctype.h>
#include <utfcpp/utf8/checked.h>
#include <vector>

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

// 中文分词(老版本的)
// void file_parse::cn_parse(const string& path) {
//     string content = get_content(path);
//     if (content.empty()) return;

//     std::vector<string> words;
//     m_tokenizer.Cut(content, words);
//     output_file(words, m_output_file_path);
// }

string  extract_content_from_item(const string & item){
    size_t content_start = item.find("<content>");
    if(content_start == string::npos) return "";
    size_t content_end = item.find("</content>");
    if(content_end == string::npos) return "";
    return item.substr(content_start+9, content_end- (content_start + 9));
}

string extract_description_from_item(const string & item) {
    size_t content_start = item.find("<description>");
    if(content_start == string::npos) return "";
    size_t content_end = item.find("</description>");
    if(content_end == string::npos) return "";
    return item.substr(content_start+13, content_end- (content_start + 13));
}

string extract_p(const string& html) {
    size_t p_start = html.find("<p");
    if (p_start == string::npos) return "";
    size_t p_tag_end = html.find(">", p_start);
    if (p_tag_end == string::npos) return "";
    size_t p_close = html.find("</p>", p_tag_end);
    if (p_close == string::npos) return "";
    return html.substr(p_tag_end + 1, p_close - (p_tag_end + 1));
}


// 中文分词（新版本：增加对网页内容的解析)
void file_parse::cn_parse(const string& path) {
    string content = get_content(path);
    std::vector<string> words;
    size_t pos = 0;
    while(1) {
        size_t item_start = content.find("<item>", pos);
        if(item_start == string::npos) break;
        size_t item_end = content.find("</item>", item_start + 6);
        string item = content.substr(item_start +6, item_end -( item_start +6));
        string clean_item = extract_content_from_item(item);
        string clean_item2 = extract_description_from_item(item);
        if(clean_item != "") {
            m_tokenizer.Cut(extract_p(clean_item),words);
        } else if(clean_item2 != ""){
            m_tokenizer.Cut(extract_p(clean_item2),words);
        } else {
            pos = item_start+7;
            continue;
        }
        pos = item_end+7;
    }
    
    output_file(words,m_output_file_path);
}



string contains_alpha(const string &s){
    string result;
    for(char e : s) {
        if(isalpha(static_cast<unsigned char>(e))) {
            result += e;
        }
    }
    return result;
}
// 英文分词
void file_parse::en_parse(const string &path) {
    string content = get_content(path);
    if(content.empty()) return;
    
    std::vector<string> words;
    string word;
    std::istringstream iss{content};
    while(iss >> word) {
        words.push_back(contains_alpha(word));
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
bool contains_non_chinese(const std::string& str) {
    try {
        auto it = str.begin();
        auto end = str.end();
        while (it != end) {
            uint32_t code_point = utf8::next(it, end);
            
            // 中文字符的Unicode范围: 0x4E00-0x9FFF, 0x3400-0x4DBF, 0x20000-0x2A6DF, 0x2A700-0x2B73F, etc.
            bool is_chinese = (code_point >= 0x4E00 && code_point <= 0x9FFF) || 
                             (code_point >= 0x3400 && code_point <= 0x4DBF) ||
                             (code_point >= 0x20000 && code_point <= 0x2A6DF) ||
                             (code_point >= 0x2A700 && code_point <= 0x2B73F) ||
                             (code_point >= 0x2B740 && code_point <= 0x2B81F) ||
                             (code_point >= 0x2B820 && code_point <= 0x2CEAF) ||
                             (code_point >= 0xF900 && code_point <= 0xFAFF) ||
                             (code_point >= 0x2F800 && code_point <= 0x2FA1F);
            
            // ASCII字母、数字或非中文字符
            if ((code_point < 0x80 && std::isalnum(code_point)) || !is_chinese) {
                return true;
            }
        }
    } catch (const utf8::exception&) {
        // 无效的UTF-8序列
        return true;
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
            if (stopwords.find(clean_line) == stopwords.end() && !contains_non_chinese(clean_line)) {
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
            // if(!contains_alpha(line)) continue;
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
        string clean_word = trim(word);
        if (!clean_word.empty()) {
            outfile << clean_word << "\n";
        }
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
        string clean = trim(line);
        if (!clean.empty()){
            count[line]++;
        }
        
    }
    ifs.close();

    // 生成相应的频率文件
    std::ofstream count_file(path);
    for(auto &[key,value] : count) {
        count_file << key << " " << value << "\n";
    }
}

// 去重
std::set<string> unique_char(const string & line) {
    std::set<string> chars;
    const char * it = line.c_str();
    const char* end = line.c_str() + line.size();

    while(it != end) {
        auto start = it;
        utf8::next(it,end);
        string words = string{start, it};
        chars.insert(words);
    }
    return chars;
}

// 生成对应的行索引:中文
void file_parse::create_index(const string & input_path,const string & output_path) {
    std::ifstream infile(input_path);
    if(!infile) {
        std::cerr<< "failed to open DICT_CN !" << std::endl;
        return;
    }
    // uint32_t是用来unicode的，实际就是左边汉字，右边行号
    std::map<string , std::vector<int>> char_index;
    string line;
    // 行号从第一行开始
    int line_number = 1;
    while(getline(infile, line)) {
        auto words = unique_char(line);
        for(const auto & e : words) {
            if (!contains_non_chinese(e)) {
                char_index[e].push_back(line_number);
            }
        }
        // 每行处理完进行行号加一
        line_number++;
    }
    infile.close();
        // 到这里已经把所有数据放到char_index了，生成文件即可

        std::ofstream out(output_path);
        if(!out) {
            std::cerr << "failed to open INDEX_FILE"<< std::endl;
            return;
        }

        for(const auto & [ch, index]: char_index) {
            out << ch << " ";

            for(int num : index){
                out <<  num << " ";
            }
            out  << "\n";
        }
        out.close();
}