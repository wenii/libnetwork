set(SRC_LIST 
"ZookeeperHandle.cpp"
"ZookeeperWatcher.cpp"
"ZookeeperClient.cpp"
"ServiceDiscoveryListenner.cpp"
)

set(LIB_NAME "zk")
add_library(${LIB_NAME} ${SRC_LIST})

install(TARGETS ${LIB_NAME} DESTINATION "/home/dev/lib/zk")