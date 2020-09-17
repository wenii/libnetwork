
#include "RouterProtoPacket.h"
#include <string.h>
#include <arpa/inet.h>
void RouterProtoPacket::unpacking(uint16_t packetSize, const char* body, int bodySize)
{
	_size = packetSize;

	uint32_t gateID = 0;
	memcpy(&gateID, body, RouterProtoPacket::PACKET_GATE_ID_BYTES);
	_gateConnID = ntohl(gateID);

	uint32_t clientConnID = 0;
	memcpy(&clientConnID, body + RouterProtoPacket::PACKET_GATE_ID_BYTES, RouterProtoPacket::PACKET_CLIENT_ID_BYTES);
	_clientConnID = ntohl(clientConnID);

	_data = body + RouterProtoPacket::PACKET_GATE_ID_BYTES + RouterProtoPacket::PACKET_CLIENT_ID_BYTES;
}

void RouterProtoPacket::packing(uint32_t gateConnID, uint32_t clientConnID, const char* body, int bodySize)
{
	_gateConnID = gateConnID;
	_clientConnID = clientConnID;
	_data = body;
	_size = RouterProtoPacket::PACKET_HEAD_SIZE + bodySize;
}


uint32_t RouterProtoPacket::getGateConnID() const
{
	return _gateConnID;
}

uint32_t RouterProtoPacket::getClientConnID() const
{
	return _clientConnID;
}


const char* RouterProtoPacket::getBody() const
{
	return _data;
}

int RouterProtoPacket::getBodySize() const
{
	return _size - RouterProtoPacket::PACKET_HEAD_SIZE;
}

int RouterProtoPacket::getSize() const
{
	return _size;
}