#ifndef __ROUTER_PROTO_PACKET_H__
#define __ROUTER_PROTO_PACKET_H__
#include <stdint.h>
class RouterProtoPacket
{
public:
	static const int PACKET_SIZE_BYTES = 2;
	static const int PACKET_GATE_ID_BYTES = 4;
	static const int PACKET_CLIENT_ID_BYTES = 4;
	static const int PACKET_HEAD_SIZE = PACKET_SIZE_BYTES + PACKET_CLIENT_ID_BYTES + PACKET_GATE_ID_BYTES;

public:
	// ���
	void unpacking(uint16_t packetSize, const char* body, int bodySize);

	// ���
	void packing(uint32_t gateConnID, uint32_t clientConnID, const char* body, int bodySize);

	// ��ȡ��������ID
	uint32_t getGateConnID() const;

	// ��ȡ�ͻ�������ID
	uint32_t getClientConnID() const;

	// ��ȡbody����
	const char* getBody() const;

	// ��ȡbody��С
	int getBodySize() const;

	// ��ȡ���ܴ�С
	int getSize() const;
private:
	uint16_t _size;
	uint32_t _gateConnID;
	uint32_t _clientConnID;
	const char* _data;
};


#endif // !__ROUTER_PROTO_PACKET_H__
