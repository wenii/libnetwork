
set(SRC_LIST 
"EchoServer.cpp"
"LuaState.cpp"
"main.cpp"
)

set(OUT_FILE "echo.out")
link_directories(${CMAKE_BINARY_DIR})
add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} "/home/dev/projects/libnetwork/c96395e3-ea2a-4f36-8ab9-b7e2706016cc/out/build/Linux-Debug/liblua.a")
target_link_libraries(${OUT_FILE} dl)
#target_link_libraries(${OUT_FILE} lua)
target_link_libraries(${OUT_FILE} network)
#link_directories("{CMAKE_BINARY_DIR}../../src")
message(${CMAKE_BINARY_DIR})


INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/examples/echo)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)