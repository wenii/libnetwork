#ifndef __GATE_PROTO_PACKET_H__
#define __GATE_PROTO_PACKET_H__
#include <string>
class GateProtoPacket
{
public:
	//          Packet                 GateProtoPacket						RouterProtoPacket
	// Client------------>GateServer---------------------->RouterServer--------------------------->LogicServer
	// 协议格式
	/* GateProtoPacket
	+----------------+---------------+-----------------------+
	|  包头(包大小)  |	客户端连接ID | 		   data          |
	+----------------+---------------+-----------------------+
	|     2字节      |	    4字节    |          Packet       |
	+----------------+---------------+-----------------------+
	*/

	/* RouterProtoPacket
	+----------------+---------------+-----------------------+
	|  包头(包大小)  |	网关连接ID   | 		   data          |
	+----------------+---------------+-----------------------+
	|     2字节      |	    4字节    |    GatePotoPacket     |
	+----------------+---------------+-----------------------+
	*/

	static const int PACKET_SIZE_BYTES = 2;
	static const int PACKET_CLIENT_ID_BYTES = 4;
	static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES + PACKET_CLIENT_ID_BYTES;

public:


public:
	// 解包
	void unPacking(uint16_t packetSize, const char* body, int bodySize);

	// 打包
	void packing(uint32_t clientConnID, const char* body, int bodySize);

	int getSize() const;

	// 获取body
	const char* getBody() const;

	// 获取body大小
	int getBodySize() const;

	// 获取connID
	uint32_t getClientConnID() const;
private:
	uint16_t _size;
	uint32_t _clientConnID;
	const char* _data;
	
};

#endif // !__GATE_PROTO_PACKET_H__
