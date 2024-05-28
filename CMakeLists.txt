cmake_minimum_required(VERSION 3.27)
project(XChatServer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

## 查找和包含Boost库
#find_package(Boost 1.68.0 REQUIRED serialization)
#include_directories(${Boost_INCLUDE_DIRS})

#set(BOOST_ROOT /opt/homebrew/opt/boost@1.76/)
#set(BOOST_INCLUDEDIR /opt/homebrew/opt/boost@1.76/include)
#set(BOOST_LIBRARYDIR opt/homebrew/opt/boost@1.76/lib)

find_package(Boost 1.76)
include_directories(${Boost_INCLUDE_DIRS})

## 指定MySQL Connector/C++的头文件路径
#include_directories(/opt/homebrew/opt/mysql-connector-c++/include)
#
## 指定MySQL Connector/C++的库文件路径
#link_directories(/opt/homebrew/opt/mysql-connector-c++/lib)
#
## 添加源文件并定义可执行目标
#add_executable(XChatServer Server.cpp)
#
## 链接MySQL Connector/C++库
#target_link_libraries(XChatServer mysqlcppconn8)