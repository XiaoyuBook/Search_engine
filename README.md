# Search_engine
## v1：离线库准备
### 准备工作
> 进入lib文件夹
> #### cppjieba的安装
>  ```shell
>  unzip cppjieba.zip  
>  sudo mv ./cppjieba/include/cppjieba /usr/local/include/
>  sudo mv ./cppjieba/deps/limonp/include/limonp /usr/local/include/
》 ```
> #### utfcpp的安装
> ```shell
> tar xzvf utfcpp-4.0.6.tar.gz 
> sudo mv ./utfcpp-4.0.6/source/* /usr/local/include/utfcpp
> ```
> #### simhash的安装
> ```shell
> tar xzvf simhash-1.3.0.tar.gz
> sudo mv ./simhash-1.3.0/include/* /usr/local/include
> ```
> #### tinyxml2的安装
> ```shell
> unzip tinyxml2-master.zip && cd ./tinyxml2-master/
> mkdir build
> cd build
> cmake ..
> make
> sudo make install
> ldconfig
> ```
### 使用步骤，生成的txt文件也均在bin文件夹下面，要给到后续功能的使用
> ```shell
> mkdir build && cd build
> cmake .. && make
> cd ../bin
> ./outline_lib
> ```

## v2：网页搜索
### 模块一：关键字推荐
> 相应模块放在了candidate_seacher.cc
### 模块二：网页搜索
> 相应模块放在了web_searcher.cc
### 使用说明
> #### 服务端：
> 进入v2/Server
> ```shell
> mkdir build && cd build
> cmake .. && make
> ./search_server
> ```
> #### 客户端：
> 进入v2/client
> ```shell
> g++ client.cc -o client
> ./client 127.0.0.1 8888
> ```

## v3：LRUCache的实现
> 实现了每个低级cache向主cache进行每隔30s打补丁更新
> 对比v2使用方法相同，增加了LRUCache和main_cache来实现缓存机制,然后使用patch_register来完成同步打补丁

## 提示：为了简化代码，使用了C++17的auto 引用,如 for (const auto& [key, value] : map),如果想用C++11则需要换成.first和.second



## 一些简单的心得体会
> 1. 可以使用inline解决一部分重定义问题
> 2. 模版函数要放在.h文件里面实现
> 3. 全局变量一般是放在.cc里面定义，然后.h文件里面进行extern声明
> 4. 如果没有排序需求的话，unordered_set和unordered_map可以常用
> 5. 多注意检查输出文件是否带后缀，否则下次调用的时候又加上后缀了就会出现问题
> 6. 关于doc文档偏移值的计算，最好少数字符然后理所当然的用加减数字进行解析
> 7. 可以使用构造函数，保证每次进程启动的时候先把之前生成的文件给清空了，去除旧文件的影响