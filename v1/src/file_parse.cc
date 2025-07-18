#include "../include/file_parse.h"
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;


file_parse::file_parse(const std::vector<string>& file_array)
    : m_file_array(std::move(file_array)) {}

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
    output_file(words, "output_file.txt");
}

void file_parse::output_file(const std::vector<string>& words,
                            const string& filename) {
    std::ofstream outfile(filename, std::ios::app);
    if (!outfile) {
        std::cerr << "Error: Failed to open output file " << filename << std::endl;
        return;
    }
    
    for (const auto& word : words) {
        outfile << word << "\n";
    }
}