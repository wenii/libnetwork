set(SRC_LIST 
"LogicServer.cpp"
"main.cpp"
)

set(OUT_FILE "LogicServer.out")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} protoPacket)
target_link_libraries(${OUT_FILE} network)
link_directories("{CMAKE_BINARY_DIR}../../src")
link_directories("{CMAKE_BINARY_DIR}../../proto")

INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/service/logic)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)