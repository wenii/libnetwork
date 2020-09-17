
set(SRC_LIST
	"GateProtoPacket.cpp"
	"RouterProtoPacket.cpp"
)

add_definitions(-std=c++11)


add_library(protoPacket ${SRC_LIST})