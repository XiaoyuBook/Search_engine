# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(SearchServerProject CXX)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加头文件搜索路径
# 这样在 .cpp 文件中就可以直接 #include "header.h"
include_directories(include)

# 自动查找 src 目录下的所有源文件 (.cpp)
# 将它们的名字存放到变量 SRC_FILES 中
aux_source_directory(src SRC_FILES)

# 生成可执行文件
# 可执行文件名为 search_server
# 源文件列表为 SRC_FILES
add_executable(search_server ${SRC_FILES})

# 查找并链接 pthreads 库, 因为代码中用到了 std::thread
find_package(Threads REQUIRED)
target_link_libraries(search_server PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# 打印生成的可执行文件路径
message(STATUS "Executable is located at: ${CMAKE_BINARY_DIR}/search_server")