#include "../include/web_parse.h"
#include "../include/utils.h"
#include <fstream>
#include <math.h>
#include <utility>

using std::string;





web_parse::web_parse(const string & offset_filepath, const string & weblib_filepath,const string & keyword_filepath):
        m_offset_filepath(offset_filepath),
        m_weblib_filepath(weblib_filepath),
        m_keyword_filepath(keyword_filepath){
            // 构造函数每次先把文件清空
            std::ofstream(m_offset_filepath, std::ios::trunc).close();
            std::ofstream(m_weblib_filepath, std::ios::trunc).close();
            std::ofstream(m_keyword_filepath, std::ios::trunc).close();
        }


string strip_tags(const string& input) {
    using namespace std;
    static const regex tag("<[^>]*>");
    return regex_replace(input, tag, "");
}

string extract_p(const string& html) {
    using namespace std;
    string result;
    regex p_regex("<p[^>]*>(.*?)</p>", regex::icase); 
    auto begin = sregex_iterator(html.begin(), html.end(), p_regex);
    auto end = sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        string inner = it->str(1);
        if (inner.find("<img") != string::npos) continue;
        result += strip_tags(inner); 
        result += "\n";
    }

    return result;
}


// 提取content并且去重
std::vector<DocMeta> web_parse::generate_unique_docs(const string& file_path) {
    using namespace tinyxml2;

    XMLDocument doc;
    std::vector<DocMeta> docs;
    std::unordered_set<uint64_t> simhash_set;  // 用于内容去重


    // 处理根径标签
    if (doc.LoadFile(file_path.c_str()) != XML_SUCCESS) {
        std::cerr << "Error: Failed to load XML file: " << file_path << std::endl;
        return docs;
    }
     XMLElement* rss = doc.FirstChildElement("rss");
    if (!rss) {
        std::cerr << "Error: <rss> element not found in " << file_path << std::endl;
        return docs;
    }
    XMLElement* channel = rss->FirstChildElement("channel");
    if (!channel) {
        std::cerr << "Error: <channel> element not found in " << file_path << std::endl;
        return docs;
    }
    XMLElement* item = channel->FirstChildElement("item");
    int total_items = 0;
    int unique_docs = 0;
    while (item) {
        total_items++;
        DocMeta meta;

        // 提取元数据
        if (XMLElement* id = item->FirstChildElement("id")) 
            meta.id = id->GetText() ? id->GetText() : "";

        if (XMLElement* link = item->FirstChildElement("link"))
            meta.link = link->GetText() ? link->GetText() : "";

        if (XMLElement* title = item->FirstChildElement("title"))
            meta.title = title->GetText() ? title->GetText() : "";

        // 提取内容优先content，没有则description
        std::string raw_content;
        XMLElement* content = item->FirstChildElement("content");
        if (content && content->GetText()) {
            raw_content = content->GetText();  // 直接拿纯文本
        } else {
            XMLElement* desc = item->FirstChildElement("description");
            if (desc && desc->GetText()) {
                raw_content = desc->GetText();
            } else {
                item = item->NextSiblingElement("item");
                continue;
            }
        }
        meta.content = raw_content;

        uint64_t hashcode;
        m_hasher.make(meta.content, 3, hashcode);

        bool is_duplicate = false;
        for (const auto& existing_hash : simhash_set) {
            if (m_hasher.isEqual(existing_hash, hashcode)) {
                is_duplicate = true;
                break;
            }
        }
        if (!is_duplicate) {
            simhash_set.insert(hashcode);
            docs.push_back(meta);
            unique_docs++;
        }

        item = item->NextSiblingElement("item");
    }

  
    return docs;
}





// 生成网页库文件
void web_parse::output_weblib_and_offset(std::vector<DocMeta> &docs) {
    std::ofstream ofs_weblib(m_weblib_filepath);
    std::ofstream ofs_offset(m_offset_filepath);

    if (!ofs_weblib) {
        std::cerr << "Error: Failed to open weblib file: " << m_weblib_filepath << std::endl;
        return;
    }
    if (!ofs_offset) {
        std::cerr << "Error: Failed to open offset file: " << m_offset_filepath << std::endl;
        return;
    }

    int docid = 1;
    size_t offset = 0;

    for (const auto& doc : docs) {
        std::ostringstream oss;
        oss << "<doc>\n";
        oss << "  <docid>" << docid << "</docid>\n";
        oss << "  <title>" << doc.title << "</title>\n";
        oss << "  <link>" << doc.link << "</link>\n";
        oss << "  <content>" << doc.content << "</content>\n";
        oss << "</doc>\n";

        std::string doc_str = oss.str();
        size_t size = doc_str.size();

        ofs_offset << docid << " " << offset << " " << (offset + size) << "\n";

        // 写入weblib
        ofs_weblib << doc_str;

        offset += size;
        ++docid;
    }

    ofs_weblib.close();
    ofs_offset.close();
}

int current_docid = 1;
// 生成关键字文档：用jieba分词器
void web_parse::output_keyword(std::vector<DocMeta> &docs) {
    using std::vector;
    using std::map;
    using std::pair;

    int total_doc = docs.size();
        // 记录关键词，关键词所在文档id和tf
    map<string,vector<pair<int, double>>> tf_index;

        // 记录单词在多少个文档中出现过
    map<string, int> doc_freq;


    for (const auto& doc : docs) {
        std::vector<string> words;
        m_tokenizer.Cut(doc.content,words);
        
        // 记录单个doc中word出现的次数
        map<string, int> word_count;
        int total = 0;
        for(const auto & word: words) {
            string clean_word = trim(word);
            if(!clean_word.empty() && !contains_non_chinese(clean_word)) {
                word_count[clean_word]++;
                total++;
            }
        }


        for(const auto & [word, freq] : word_count) {
            double tf = static_cast<double>(freq) / total; 
            tf_index[word].emplace_back(current_docid,tf);
            doc_freq[word]++;
        }

        current_docid++;  // 处理下一篇文档时docid递增
    }
    // 归一化准备
    map<int, double> doc_norms;
    // 开始计算tf-idf
    for(const auto & [word, posting] : tf_index) {
        int df = doc_freq[word];
        double idf = log(static_cast<double>(total_doc)/ (df + 1));

        for(const auto [docid, tf] : posting ) {
            double tfidf = tf*idf;
            doc_norms[docid] += tfidf * tfidf;
        }
    }


    // 平方和开根号
    for(auto &[docid, weigh] :doc_norms  ) {
        weigh = sqrt(weigh);
    }


    std::ofstream ofs(m_keyword_filepath, std::ios::app);
    if (!ofs) {
        std::cerr << "Error: Failed to open keyword file: " << m_keyword_filepath << std::endl;
        return;
    }

    for (const auto& [word, posting] : tf_index) {
        int df = doc_freq[word];
        double idf = log(static_cast<double>(total_doc) / (df + 1));
        
        ofs << word;
        for (const auto& [docid, tf] : posting) {
            double tfidf = tf * idf;
            double normalized_tfidf = tfidf / doc_norms[docid];
            ofs << " " << docid << " " << normalized_tfidf;
        }
        ofs << "\n";
    }

    ofs.close();
}



void web_parse::generate_files_from_list(const std::vector<std::string>& file_list) {
    std::vector<DocMeta> all_docs;

    for (const auto& path : file_list) {
        auto docs = generate_unique_docs(path);
        std::cout << path << ": parsed " << docs.size() << " docs" << std::endl;
        all_docs.insert(all_docs.end(), docs.begin(), docs.end());
    }
    output_weblib_and_offset(all_docs);
    output_keyword(all_docs);
}
