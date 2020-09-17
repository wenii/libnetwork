#include "GateProtoPacket.h"
#include <string.h>
#include <arpa/inet.h>

void GateProtoPacket::unPacking(uint16_t packetSize, const char* body, int bodySize)
{	
	_size = packetSize;

	uint32_t clientConnID = 0;
	memcpy(&clientConnID, body, GateProtoPacket::PACKET_CLIENT_ID_BYTES);
	_clientConnID = ntohl(clientConnID);

	_data = body + GateProtoPacket::PACKET_CLIENT_ID_BYTES;
}

void GateProtoPacket::packing(uint32_t clientConnID, const char* body, int bodySize)
{
	_size = PACKET_SIZE_BYTES + PACKET_CLIENT_ID_BYTES + bodySize;
	_clientConnID = clientConnID;
	_data = body;
}

const char* GateProtoPacket::getBody() const
{
	return _data;
}

int GateProtoPacket::getSize() const
{
	return _size;
}

int GateProtoPacket::getBodySize() const
{
	return _size - GateProtoPacket::PACKET_HEAD_SIZE;
}

uint32_t GateProtoPacket::getClientConnID() const
{
	return _clientConnID;
}