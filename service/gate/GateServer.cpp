#include "GateServer.h"
#include "../../src/Packet.h"
#include "../../proto/GateProtoPacket.h"
#include "../../src/Types.h"
using namespace libnetwork;

void GateServer::onPacket(ConnID connID, const Packet& packet)
{
	if (/*是客户端发送过来的消息*/)
	{
		// 修改协议，增加本进程标识，连接ID
		GateProtoPacket gateProtoPacket;
		gateProtoPacket.packing(connID, packet.getBody(), packet.getBodySize());
		
		// 转发包到路由服
		uint16_t packetSize = gateProtoPacket.getSize();
		packetSize = htns(&packetSize);
		ConnID routerID = 0;
		send(routerID, (char*)&packetSize, sizeof(GateProtoPacket::PACKET_SIZE_BYTES));

		uint32_t clientConnID = gateProtoPacket.getClientConnID();
		clientConnID = htnl(&clientConnID);
		send(routerID, (char*)&clientConnID, sizeof(GateProtoPacket::PACKET_CLIENT_ID_BYTES));

		send(routerID, gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
	}
	else
	{
		// 从路由服务来的消息
		GateProtoPacket gateProtoPacket;
		gateProtoPacket.unPacking(packet.getSize(), packet.getBody(), packet.getBodySize());

		// 转发给客户端
		const ConnID clientConnID = gateProtoPacket.getClientConnID();
		uint16_t packetSize = gateProtoPacket.getSize() - GateProtoPacket::PACKET_CLIENT_ID_BYTES;
		packetSize = htns(&packetSize);
		send(connID, (char*)&packetSize, sizeof(Packet::PACKET_SIZE_BYTES));
		send(connID, gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
	}
}

void GateServer::onAccept(long long clientID)
{

}

void GateServer::onDisconnect(long long clientID)
{

}

void GateServer::update()
{

}