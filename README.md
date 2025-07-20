# Search_engine
## 准备工作
> 解压lib中的两个文件
> ### jieba工具包
> 1. mv ./cppjieba/include/cppjieba /usr/local/include/
> 2. mv ./cppjieba/deps/limonp/include/limonp /usr/local/include/cppjieba
> ### utf8
> tar xzvf simhash-1.3.0.tar.gz
> sudo mv ./simhash-1.3.0/include/* /usr/local/include
> ### tinyxml2的安装
> ```shell
> unzip tinyxml2-master.zip && cd ./tinyxml2-master/
> mkdir build
> cd build
> cmake ..
> make
> sudo make install
> ldconfig
> ```