#include "../include/file_scanner.h"

using std::string;
using std::vector;



file_scanner::file_scanner(const char *scanner_path) 
    : m_scanner_path(scanner_path) {}

file_scanner::~file_scanner() {}

void file_scanner::start() {
    scanner(m_scanner_path);
}

const std::vector<std::string>& file_scanner::get_files() const {
    return filename;
}

void file_scanner::scanner(const std::string& current_dir) {
    DIR *dir = opendir(current_dir.c_str());
    if (!dir) {
        perror("opendir failed");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        std::string full_path = current_dir + "/" + entry->d_name;
        struct stat statbuf;
        if (stat(full_path.c_str(), &statbuf) == -1) {
            std::cerr << "stat failed" << std::endl;
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            scanner(full_path);
        } else {
            filename.push_back(full_path);
        }
    }
    closedir(dir);
}