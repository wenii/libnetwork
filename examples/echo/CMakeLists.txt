
set(SRC_LIST 
"EchoServer.cpp"
"main.cpp"
)

set(OUT_FILE "echo.out")

add_executable(${OUT_FILE} ${SRC_LIST})
target_link_libraries(${OUT_FILE} network)
link_directories("{CMAKE_BINARY_DIR}../../src")

INSTALL(FILES "../../server.cfg" DESTINATION ${CMAKE_BINARY_DIR}/examples/echo)
INSTALL(TARGETS ${OUT_FILE} RUNTIME DESTINATION /home/dev/projects)