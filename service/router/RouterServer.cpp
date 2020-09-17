#include "RouterServer.h"
#include "../../proto/GateProtoPacket.h"
#include "../../src/Packet.h"
#include "../../proto/RouterProtoPacket.h"
#include <arpa/inet.h>

void RouterServer::onPacket(ConnID connID, const Packet& packet)
{
	if (/*从网关来的消息*/1)
	{	
		GateProtoPacket gateProtoPacket;
		gateProtoPacket.unPacking(packet.getSize(), packet.getBody(), packet.getBodySize());

		router(connID, gateProtoPacket.getClientConnID(), gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
	}
	else if (/*从逻辑服过来的消息*/1)
	{
		RouterProtoPacket routerPacket;
		routerPacket.unpacking(packet.getSize(), packet.getBody(), packet.getBodySize());

		router(routerPacket.getGateConnID(), routerPacket.getClientConnID(), routerPacket.getBody(), routerPacket.getBodySize());
	}
}

void RouterServer::onAccept(long long clientID)
{

}

void RouterServer::onDisconnect(long long clientID)
{

}

void RouterServer::update()
{

}

void RouterServer::router(ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	// 路由消息到网关或者逻辑服
}

void RouterServer::sendToLogicServer(ConnID logicID, ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	RouterProtoPacket packet;
	packet.packing(gateID, clientID, data, dataSize);

	uint16_t packetSize = packet.getSize();
	uint32_t gateID = packet.getGateConnID();
	uint32_t clientID = packet.getClientConnID();
	packetSize = htons(packetSize);
	gateID = htonl(gateID);
	clientID = htonl(clientID);

	send(logicID, (char*)&packetSize, RouterProtoPacket::PACKET_SIZE_BYTES);
	send(logicID, (char*)&gateID, RouterProtoPacket::PACKET_GATE_ID_BYTES);
	send(logicID, (char*)&clientID, RouterProtoPacket::PACKET_CLIENT_ID_BYTES);
	send(logicID, data, dataSize);

}

void RouterServer::sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	GateProtoPacket packet;
	packet.packing(clientID, data, dataSize);
	uint16_t packetSize = packet.getSize();
	uint32_t clientID = packet.getClientConnID();
	packetSize = htons(packetSize);
	clientID = htonl(clientID);

	send(gateID, (char*)&packetSize, GateProtoPacket::PACKET_SIZE_BYTES);
	send(gateID, (char*)&clientID, GateProtoPacket::PACKET_CLIENT_ID_BYTES);
	send(gateID, data, dataSize);
}