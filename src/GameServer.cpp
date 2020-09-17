#include "GameServer.h"
#include "Packet.h"
#include "Log.h"

using namespace libnetwork;

int GameServer::onRecv(ConnID connID, const char* buf, int size)
{
	int handleSize = 0;
	while (size > Packet::PACKET_HEAD_SIZE)
	{
		const int packetSize = Packet::parsePacketSize(buf, size);
		if (Packet::isPacketLegal(packetSize))
		{
			if (packetSize <= size)
			{
				Packet packet;
				packet.Unpacking(buf, packetSize);
				onPacket(connID, packet.getBody(), packet.getBodySize());
				handleSize += packetSize;
				size -= packetSize;
				buf += handleSize;
			}
			else
			{
				break;
			}
		}
		else
		{
			Log::info("GameServer::onRecv packet size is illegal. connID:%lld", connID);

			// 主动关闭连接
			disconnect(connID);

			break;
		}
	}
	return handleSize;
}

void GameServer::sendProto(ConnID connID, const std::string& protoName, const std::string& protoData)
{
	Packet packet;
	packet.Packing(protoName, protoData);
	if (!send(connID, packet.getData().c_str(), packet.getSize())
	{
		Log::info("GameServer::sendProto send proto:%s failed. connID:%lld", protoName.c_str(), connID);
	}
}
