#include "../include/candidate_searcher.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "utfcpp/utf8.h"
#include <set>
#include <iostream>


using std::ifstream;
using std::istringstream;
using std::pair;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;
using json = nlohmann::json;

candidate_searcher::candidate_searcher(const string& msg,const string& index_path,const string& dict_path)
    : m_msg(msg), m_index_path(index_path), m_dict_path(dict_path) {}

    vector<json> candidate_searcher::search(int k) const {
    vector<string> keywords = cut_keywords(m_msg);
    vector<pair<string, int>> candidate = get_candidate(keywords);

    set<pair<string, int>, candidate_comparator> sorted_set{candidate_comparator(m_msg)};
    for (const auto& item : candidate) {
        sorted_set.insert(item);
    }

    vector<json> result;
    int count = 0;
    for (const auto& [word, freq] : sorted_set) {
        if (count++ >= k) break;
        json j;
        j["word"] = word;
        j["frequency"] = freq;
        j["distance"] = edit_distance(m_msg, word);
        result.push_back(j);
    }
    return result;
}

vector<string> candidate_searcher::cut_keywords(const string& text) {
    vector<string> result;
    const char* it = text.c_str();
    const char* end = it + text.size();
    while (it != end) {
        auto start = it;
        utf8::next(it, end);
        result.emplace_back(start, it);
    }
    return result;
}

vector<pair<string, int>> candidate_searcher::get_candidate(const vector<string>& keywords) const {
    unordered_map<string, vector<int>> index_map;
    ifstream idx_ifs(m_index_path);
    string line;

    while (getline(idx_ifs, line)) {
        istringstream iss(line);
        string key;
        iss >> key;
        int pos;
        while (iss >> pos) {
            index_map[key].push_back(pos);
        }
    }

    vector<pair<string, int>> dict_vec;
    ifstream dict_ifs(m_dict_path);
    while (getline(dict_ifs, line)) {
        istringstream iss(line);
        string word;
        int freq;
        if (iss >> word >> freq) {
            dict_vec.emplace_back(word, freq);
        }
    }

    vector<pair<string, int>> result;
    for (const auto& ch : keywords) {
        auto it = index_map.find(ch);
        if (it != index_map.end()) {
            for (int pos : it->second) {
                if (pos >= 0 && pos < static_cast<int>(dict_vec.size())) {
                    result.push_back(dict_vec[pos]);
                }
            }
        }
    }

    return result;
}

int candidate_searcher::edit_distance(const string& lhs, const string& rhs) {
    vector<string> lhs_chars = cut_keywords(lhs);
    vector<string> rhs_chars = cut_keywords(rhs);
    size_t m = lhs_chars.size(), n = rhs_chars.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (lhs_chars[i - 1] == rhs_chars[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }
    return dp[m][n];
}



// // 模块单独测试
// int main() {
//     // 模拟输入数据
//     std::string query = "一二三";
//     std::string index_path = "../../../v1/bin/index_cn.txt";   
//     std::string dict_path = "../../../v1/bin/dict_cn.txt";     


//     candidate_searcher searcher(query, index_path, dict_path);


//     int k = 5;
//     std::vector<nlohmann::json> result = searcher.search(k);

//     // 输出结果
//     for (const auto& item : result) {
//         std::cout << item.dump(4) << std::endl;
//     }

//     return 0;
// }



// 记录原字符串版本（多余了）
// std::unordered_map<string,vector<std::pair<string,int>>> get_candidate(
//     const string & indexpath, const string& dictpath, vector<string> msg)
// {
//     std::unordered_map<string, vector<int>> idx; // 读取索引映射
//     std::ifstream idx_ifs(indexpath);
//     string line;

//     // 读取索引文件
//     while(getline(idx_ifs, line)) {
//         std::istringstream idx_iss(line);
//         string chinese_char;
//         idx_iss >> chinese_char;
//         int num;
//         vector<int> numbers;
//         while (idx_iss >> num) {
//             numbers.push_back(num);
//         }
//         idx[chinese_char] = numbers;
//     }

//     // 读取字典文件
//     vector<std::pair<string, int>> dict_vec;
//     std::ifstream dict_ifs(dictpath);
//     while(getline(dict_ifs, line)) {
//         std::istringstream dict_iss(line);
//         string word;
//         int freq;
//         if (dict_iss >> word >> freq) {
//             dict_vec.emplace_back(word, freq);
//         }
//     }

//     // 构建候选词表
//     std::unordered_map<string, vector<std::pair<string, int>>> candidate;
//     for (const auto& word : msg) {
//         auto it = idx.find(word);
//         if (it != idx.end()) {
//             for (int e : it->second) {
//                 if (e >= 0 && e < dict_vec.size()) {
//                     candidate[word].push_back(dict_vec[e]);
//                 }
//             }
//         }
//     }

//     return candidate;
// }

// 不需要留下来 msg中的字符