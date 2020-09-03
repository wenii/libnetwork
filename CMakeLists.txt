# CMakeList.txt: libnetwork 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project(libnetwork)

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# 将源代码添加到此项目的可执行文件。
add_subdirectory("src")
add_subdirectory("test")




# TODO: 如有需要，请添加测试并安装目标。
