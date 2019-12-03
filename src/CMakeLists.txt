
set(SRC_LIST
	"${CMAKE_CURRENT_LIST_DIR}/socket/Socket.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/socket/Log.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/socket/SocketUtils.cpp"
)

add_library(network ${SRC_LIST})