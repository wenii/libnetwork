set(SRC_LIST 
"LogicServer.cpp"
"main.cpp"
"LuaState.cpp"
)

set(OUT_FILE "LogicServer.out")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} "/home/dev/projects/libnetwork/c96395e3-ea2a-4f36-8ab9-b7e2706016cc/out/build/Linux-Debug/liblua.a")
target_link_libraries(${OUT_FILE} dl)
target_link_libraries(${OUT_FILE} protoPacket)
target_link_libraries(${OUT_FILE} network)
link_directories("{CMAKE_BINARY_DIR}../../src")
link_directories("{CMAKE_BINARY_DIR}../../proto")

INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/server/logic)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)
install(DIRECTORY
	script
	DESTINATION
	${CMAKE_BINARY_DIR}/server/logic)