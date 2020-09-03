#include "Packet.h"
#include <arpa/inet.h>
#include <string.h>
using namespace libnetwork;

Packet::Packet()
	: _size(0)
{

}

int Packet::parsePacketSize(const char* buf)
{
	int packetSize = 0;
	memcpy(&packetSize, buf, Packet::PACKET_SIZE_BYTES);
	return ntohl(packetSize);
}

bool Packet::isPacketLegal(int size)
{
	if (size < Packet::PACKET_HEAD_SIZE)
	{
		return false;
	}
	return true;
}

void Packet::Unpacking(const char* buf, int size)
{
	// 包大小
	_size = size;
	
	// 协议名称
	short protoNameLen = 0;
	memcpy(&protoNameLen, buf + Packet::PACKET_SIZE_BYTES, Packet::PROTO_NAME_BYTES);
	protoNameLen = ntohs(protoNameLen);

	char protoName[1024] = { 0 };
	memcpy(protoName, buf + Packet::PACKET_SIZE_BYTES + Packet::PROTO_NAME_BYTES, protoNameLen);
	_protoName = protoName;

	// 协议数据
	_protoData.append(buf + Packet::PACKET_SIZE_BYTES + Packet::PROTO_NAME_BYTES + protoNameLen, size - Packet::PACKET_HEAD_SIZE + protoNameLen);
}

void Packet::Packing(const std::string& protoName, const std::string& protoData)
{
	const short protoNameLen = protoName.size();
	// 包大小
	_size = Packet::PACKET_HEAD_SIZE + protoNameLen + protoData.size();
	
	// 协议名称
	_protoName = protoName;

	// 包数据
	const int size = htonl(_size);
	const short nameLen = htons(protoNameLen);
	_packetData.append((const char*)&size, Packet::PACKET_SIZE_BYTES);
	_packetData.append((const char*)&nameLen, Packet::PROTO_NAME_BYTES);
	_packetData.append(protoData);
}

int Packet::getSize() const
{
	return _size;
}

const std::string& Packet::getProtoName() const
{
	return _protoName;
}

const std::string& Packet::getProtoData() const
{
	return _protoData;
}

const std::string& Packet::getData() const
{
	return _packetData;
}