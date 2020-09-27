# CMakeList.txt: libnetwork 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project(libnetwork)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
include_directories("thirdparty/lua/include")
include_directories("thirdparty/zookeeper/include")
include_directories("component/zookeeper")

# 将源代码添加到此项目的可执行文件。
add_subdirectory("src")
add_subdirectory("test")
add_subdirectory("examples/echo")
add_subdirectory("proto")
add_subdirectory("service/gate")
add_subdirectory("service/router")
add_subdirectory("service/logic")
add_subdirectory("component/zookeeper")





# TODO: 如有需要，请添加测试并安装目标。
