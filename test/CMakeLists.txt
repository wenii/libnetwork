

set(SRC_LIST "test.cpp")


add_executable("test.out" ${SRC_LIST})
target_link_libraries("test.out" network)

