cmake_minimum_required(VERSION 3.10)
project(FileScannerDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(include)

add_executable(outline_lib 
    src/main.cc
    src/file_scanner.cc
    src/file_parse.cc
    src/web_parse.cc
)

find_package(tinyxml2 REQUIRED)   # 找系统安装的tinyxml2

target_link_libraries(words_count PRIVATE tinyxml2)  # 链接tinyxml2库
