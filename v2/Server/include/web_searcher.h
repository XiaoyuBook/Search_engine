#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "cppjieba/Jieba.hpp"
using std::string;
using std::vector;

class web_searcher {
public:
    web_searcher(const string& msg, const string& idxpath);
    std::vector<std::pair<int, double>> search_topk(int k, const std::string& stopwords_path);

private:
    string m_msg;
    string m_idxpath;
    string m_libpath;
    cppjieba::Jieba m_tokenizer;
    vector<string> m_wordarry;


    void cut_keywords();
    void filter_stopwords(const string& stopwords_path);
    std::map<string, double> compute_base_weight();
    std::unordered_map<int, vector<std::pair<string, double>>> search_containing_docs();
    vector<std::pair<int, double>> rank_documents_by_cosine(const std::map<string, double>& base_weight, const std::unordered_map<int, vector<std::pair<string, double>>>& doc_keywords);

};
