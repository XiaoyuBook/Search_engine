#include "../include/file_scanner.h"
#include "../include/file_parse.h"

#define TYPE_CN 1
#define TYPE_EN 2

int main() {
    file_scanner cn_scanner("../corpus/webpages");
    cn_scanner.start();
    auto cn_filename = cn_scanner.get_files();
    file_parse cn(cn_filename,"./cnoutput.txt");
    cn.start(1);
    cn.file_filter("../stopwords/cn_stopwords.txt", TYPE_CN);
    cn.count_words("dict_cn");
    cn.create_index("dict_cn", "index_cn");

    // 英语部分测试，后面项目用不到，因此注释
    // file_scanner en_scanner("../corpus/EN");
    // en_scanner.start();
    // auto en_filename = en_scanner.get_files();
    // file_parse en(en_filename,"./enoutput.txt");
    // en.start(2);
    // en.file_filter("../stopwords/en_stopwords.txt", TYPE_EN);
    // en.count_words("dict_en");
    // std::cout << "en finished" << std::endl;
    return 0;
}