set(SRC_LIST 
"ZookeeperHandle.cpp"
"ZookeeperWatcher.cpp"
)

set(LIB_NAME "zk")

add_library(${LIB_NAME} ${SRC_LIST})
#target_link_libraries(${LIB_NAME} libzookeeper_mt)
install(TARGETS ${LIB_NAME} DESTINATION "/home/dev/lib/zk")