#include "../include/file_scanner.h"
#include "../include/file_parse.h"



int main() {
    file_scanner scanner("../corpus/CN/");
    scanner.start();
    auto filename = scanner.get_files();
    file_parse fp(filename,"./output.txt");
    fp.start();
    fp.file_filter("../corpus/stopwords/cn_stopwords.txt");
    return 0;
}