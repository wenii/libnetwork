
set(SRC_LIST 
"GateServer.cpp"
"main.cpp"
)

set(OUT_FILE "GateServer.out")


link_directories("${CMAKE_SOURCE_DIR}/thirdparty/zookeeper/lib")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} protoPacket)
target_link_libraries(${OUT_FILE} network)
target_link_libraries(${OUT_FILE} zk)
target_link_libraries(${OUT_FILE} zookeeper_mt.a)
target_link_libraries(${OUT_FILE} pthread)

link_directories("${CMAKE_BINARY_DIR}../../src")
link_directories("${CMAKE_BINARY_DIR}../../proto")
link_directories("${CMAKE_BINARY_DIR}../../zookeeper")

MESSAGE("CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR}/thirdparty/zookeeper/lib")

INSTALL(FILES "${CMAKE_SOURCE_DIR}/thirdparty/zookeeper/lib/libzookeeper_mt.so.2" DESTINATION ${CMAKE_BINARY_DIR}/service/gate)
INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/service/gate)
INSTALL(FILES "gate.cfg" DESTINATION ${CMAKE_BINARY_DIR}/service/gate)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)