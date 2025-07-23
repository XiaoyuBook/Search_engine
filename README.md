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
> 对比v2使用方法相同，增加了LRUCache.cc和Main_cache.cc来实现缓存机制