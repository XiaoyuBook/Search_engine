#pragma once
#include <string>
#include "utfcpp/utf8.h"



inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

inline bool contains_non_chinese(const std::string& str) {
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