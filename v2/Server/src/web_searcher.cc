#include "../include/web_searcher.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <unordered_set>
#include <algorithm>

using json = nlohmann::json;

web_searcher::web_searcher(const std::string& msg, const std::string& idxpath)
    : m_msg(msg), m_idxpath(idxpath),
      m_tokenizer() {}

// 切分用户发来的信息
void web_searcher::cut_keywords() {
    m_tokenizer.Cut(m_msg, m_wordarry);
}

// 过滤停用词
void web_searcher::filter_stopwords(const std::string& stopwords_path) {
    std::ifstream ifs(stopwords_path);
    std::unordered_set<std::string> stopwords;
    std::string word;
    while (ifs >> word) {
        stopwords.insert(word);
    }

    m_wordarry.erase(std::remove_if(m_wordarry.begin(), m_wordarry.end(),
        [&stopwords](const std::string& s) {
            return stopwords.count(s) || s.empty();
        }), m_wordarry.end());
}

// 计算基准权重
std::map<std::string, double> web_searcher::compute_base_weight() {
    std::unordered_map<std::string, int> freq;
    for(const auto& term : m_wordarry) {
        freq[term]++;
    }
    int total = m_wordarry.size();
    int unique_terms = freq.size();
    double idf = log(static_cast<double>(unique_terms)) + 1.0;

    std::map<std::string, double> weight;
    for(const auto& [key, value] : freq) {
        double tf = static_cast<double>(value) / total;
        weight[key] = tf * idf;
    }

    double norm = 0.0;
    for(const auto& [_, w] : weight) {
        norm += w * w;
    }
    norm = sqrt(norm);
    for(auto& [_, w] : weight) {
        w /= norm;
    }
    return weight;
}
// 寻找全部文档
std::unordered_map<int, std::vector<std::pair<std::string, double>>>web_searcher::search_containing_docs() {
    std::ifstream ifs(m_idxpath);
    std::string line;
    std::unordered_map<std::string, std::vector<std::pair<int,double>>> reverse_idx;
    // 倒插索引库读取到内存
    while(getline(ifs,line)) {
        std::istringstream iss(line);
        std::string keyword;
        int docid;
        double weight;
        iss >> keyword;
        while(iss >> docid >> weight) {
            reverse_idx[keyword].emplace_back(docid, weight);
        }
    }
    // 取出 用户搜索所含关键字的文档
    std::unordered_map<std::string, std::vector<std::pair<int,double>>> keyword_appear;
    for(const auto& kw : m_wordarry) {
        auto it = reverse_idx.find(kw);
        if (it == reverse_idx.end()) return {};
        keyword_appear[kw] = it->second;
    }



    // 将文档的主键换成key--->[单词,权重]
    std::unordered_map<int, std::vector<std::pair<std::string, double>>> docid_to_keywords;
    for(const auto& [kw, doclist] : keyword_appear) {
        for(const auto& [docid, weight] : doclist) {
            docid_to_keywords[docid].emplace_back(kw, weight);
        }
    }

    // 去除未包含所有关键字的文档
    for(auto it = docid_to_keywords.begin(); it != docid_to_keywords.end(); ) {
        std::unordered_set<std::string> kw_set;
        for(const auto& [kw, _] : it->second) {
            kw_set.insert(kw);
        }
        bool contains_all = true;
        for(const auto& kw : m_wordarry) {
            if (!kw_set.count(kw)) {
                contains_all = false;
                break;
            }
        }
        if (!contains_all) it = docid_to_keywords.erase(it);
        else it++;
    }

    return docid_to_keywords;
}

// 计算余弦相似度并排序
std::vector<std::pair<int, double>> web_searcher::rank_documents_by_cosine(
    const std::map<std::string, double>& base_weight,
    const std::unordered_map<int, std::vector<std::pair<std::string, double>>>& doc_keywords) {
    std::vector<std::pair<int, double>> results;
    for (const auto& [docid, kw_vec] : doc_keywords) {
        double dot = 0.0;
        for (const auto& [kw, w] : kw_vec) {
            auto it = base_weight.find(kw);
            if (it != base_weight.end()) {
                dot += w * it->second;
            }
        }
        results.emplace_back(docid, dot);
    }
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    return results;
}

// 汇总
std::vector<std::pair<int, double>> web_searcher::search_topk(int k, const std::string& stopwords_path) {
    // 1. 分词
    cut_keywords();
    // 2. 过滤停用词
    filter_stopwords(stopwords_path);
    // 如果没有关键词，直接返回空
    if (m_wordarry.empty()) {
        return {};
    }
    // 3. 计算基准向量
    std::map<std::string, double> base_w = compute_base_weight();

    // 4. 查询包含所有关键词的文档
    auto doc_keywords = search_containing_docs();
    if (doc_keywords.empty()) {
        return {};
    }
    // 5. 计算余弦相似度并排序
    auto ranked = rank_documents_by_cosine(base_w, doc_keywords);
    // 6. 返回前k个
    if (ranked.size() > static_cast<size_t>(k)) {
        ranked.resize(k);
    }
    return ranked;
}



// int main() {
//     std::string query = "从《故宫日历》成为“网红”到《国家宝藏》《唐宫夜宴》和三星堆的“出圈”，每一年，博物馆和公众似乎又走近了一些。今年国际博物馆日的主题为“博物馆的未来：恢复与重塑”，后疫情时代，博物馆“出圈”之后的路该如何走，怎样让传统接续未来，期待从业者开启新思考、探索新模式、提出新方案，让传统文化更好地抵达公众";
//     std::string idx_path = "../../../v1/bin/keywords.txt";
//     std::string stopwords_path = "../../../v1/stopwords/cn_stopwords.txt";

//     web_searcher searcher(query, idx_path);
//     int top_k = 5;
//     auto results = searcher.search_topk(top_k, stopwords_path);

//     if (results.empty()) {
//         std::cout << "没有找到包含所有关键字的相关文档" << std::endl;
//     } else {
//         std::cout << "Top " << top_k << " 相关文档（docid, 余弦相似度）：" << std::endl;
//         for (const auto& [docid, score] : results) {
//             std::cout << "DocID: " << docid << ", Score: " << score << std::endl;
//         }
//     }

//     return 0;
// }