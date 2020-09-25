
set(SRC_LIST 
"GateServer.cpp"
"ZookeeperClient.cpp"
"main.cpp"
)

set(OUT_FILE "GateServer.out")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} protoPacket)
target_link_libraries(${OUT_FILE} network)
link_directories("{CMAKE_BINARY_DIR}../../src")
link_directories("{CMAKE_BINARY_DIR}../../proto")

INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/service/gate)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)