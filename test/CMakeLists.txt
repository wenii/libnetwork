 

set(SRC_LIST 
"test.cpp"
"TestServer.cpp"
"TestTimer.cpp"
)

set(OUT_FILE "test.out")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} network)
link_directories("{CMAKE_BINARY_DIR}../src")

INSTALL(FILES "../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/test)
INSTALL(TARGETS test.out RUNTIME DESTINATION /home/dev/projects)


