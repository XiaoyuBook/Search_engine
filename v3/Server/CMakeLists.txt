
cmake_minimum_required(VERSION 3.10)

project(SearchServerProject CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories(include)


aux_source_directory(src SRC_FILES)


add_executable(search_server ${SRC_FILES})

find_package(Threads REQUIRED)
target_link_libraries(search_server PRIVATE ${CMAKE_THREAD_LIBS_INIT})

message(STATUS "Executable is located at: ${CMAKE_BINARY_DIR}/search_server")