cmake_minimum_required(VERSION 3.10)

project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/usr/local/Homebrew/include)
include_directories(include)

file(GLOB SOURCES "./src/*.cpp")


# 启用 AddressSanitizer 和 Undefined Behavior Sanitizer
#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    #set(SANITIZER_FLAGS "-fsanitize=address" "-fsanitize=undefined")
    #add_compile_options(${SANITIZER_FLAGS})
    #add_link_options(${SANITIZER_FLAGS})
#endif()

# 设置 Debug 模式以生成调试信息
#set(CMAKE_BUILD_TYPE Debug)


add_executable(server ${SOURCES})