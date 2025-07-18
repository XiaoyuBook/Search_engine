#pragma once

#include <vector>
#include <string>
#include "file_scanner.h"
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>


using std::vector;
using std::string;


class file_scanner {
public:
    file_scanner(const char *scanner_path);
    ~file_scanner();

    void start();
    const std::vector<std::string>& get_files() const;

private:

    void scanner(const std::string& current_dir);
    std::vector<std::string> filename;
    const char* m_scanner_path;

};