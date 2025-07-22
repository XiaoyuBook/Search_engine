#pragma once
#include <string>
#include <vector>
#include <utility>
#include "nlohmann/json.hpp"
using std::string;
class candidate_searcher {
public:
    candidate_searcher(const string& msg,
                    const string& index_path,
                    const string& dict_path);

    std::vector<nlohmann::json> search(int k) const;

private:
    string m_msg;
    string m_index_path;
    string m_dict_path;

    static std::vector<string> cut_keywords(const string& text);
    std::vector<std::pair<string, int>> get_candidate(const std::vector<string>& keywords) const;
    static int edit_distance(const string& lhs, const string& rhs);

    struct candidate_comparator {
        string msg;
        explicit candidate_comparator(const string& ele) : msg(ele) {}

        bool operator()(const std::pair<string, int>& lhs,
                        const std::pair<string, int>& rhs) const {
            int ldis = edit_distance(msg, lhs.first);
            int rdis = edit_distance(msg, rhs.first);

            if (ldis != rdis)
                return ldis < rdis;
            else if (lhs.second != rhs.second)
                return lhs.second > rhs.second;
            else
                return lhs.first < rhs.first;
        }
    };
};