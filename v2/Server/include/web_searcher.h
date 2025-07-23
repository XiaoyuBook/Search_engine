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
   
    web_searcher(
        const std::string& msg, 
        const std::string& idx_path,
        const std::string& weblib_path,
        const std::string& offset_path,
        const std::string& stopwords_path
    );
    std::vector<string> output(int k);

    std::unordered_map<int,std::pair<size_t,size_t>> get_offset_by_docid(
        const std::vector<std::pair<int, double>> & temp);
    
    std::vector<string> get_content_by_docid (
        const std::unordered_map<int,
        std::pair<size_t,size_t>>& map,
        std::vector<std::pair<int, double>>& rank);
private:
    string m_msg;
    string m_idx_path;
    string m_weblib_path;
    string m_offset_path;
    string m_stopwords_path;
    cppjieba::Jieba m_tokenizer;
    vector<string> m_wordarry;


    void cut_keywords();
    void filter_stopwords();
    std::map<string, double> compute_base_weight();
    std::unordered_map<int, vector<std::pair<string, double>>> search_containing_docs();
    vector<std::pair<int, double>> rank_documents_by_cosine(const std::map<string, double>& base_weight, const std::unordered_map<int, vector<std::pair<string, double>>>& doc_keywords);

};
