#include "Packet.h"
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
using namespace libnetwork;

Packet::Packet()
	: _size(0)
	, _buffer(nullptr)
{

}

bool Packet::Unpacking(const char* buf, int size)
{
	assert(size >= Packet::PACKET_HEAD_SIZE);

	uint16_t packetSize = 0;
	memcpy(&packetSize, buf, Packet::PACKET_SIZE_BYTES);
	packetSize = ntohs(packetSize);
	
	if (packetSize > Packet::PACKET_HEAD_SIZE && packetSize < Packet::PACKET_MAX_SIZE)
	{
		_size = packetSize;
		_buffer = buf;
		return true;
	}
	return false;
}

void Packet::Packing(const char* data, int size)
{
}

int Packet::getSize() const
{
	return _size;
}

const char* Packet::getBuffer() const
{
	return _buffer;
}

int Packet::getBodySize() const
{
	return _size - Packet::PACKET_HEAD_SIZE;
}
const char* Packet::getBody() const
{
	return _buffer + Packet::PACKET_HEAD_SIZE;
}

