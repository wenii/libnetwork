
set(SRC_LIST
	"${CMAKE_CURRENT_LIST_DIR}/Log.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Socket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/EpollApi.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/EventLoop.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TcpServer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Connection.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/IOBuffer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/SelectApi.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Timer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/TimingWheel.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Packet.cpp"
	#"${CMAKE_CURRENT_LIST_DIR}/GameServer.cpp"
)

add_definitions(-std=c++11)


add_library(network ${SRC_LIST})
install(TARGETS network DESTINATION "/home/dev/lib/network")
install(FILES TcpServer.h DESTINATION "/home/dev/include/network")