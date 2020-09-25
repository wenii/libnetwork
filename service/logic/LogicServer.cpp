#include "LogicServer.h"
#include "../../proto/RouterProtoPacket.h"
#include "../../src/Packet.h"
#include <arpa/inet.h>
void LogicServer::onPacket(ConnID connID, const Packet& packet)
{
	RouterProtoPacket routerPacket;
	routerPacket.unpacking(packet.getSize(), packet.getBody(), packet.getBodySize());
	onHandleMsg(connID, routerPacket.getGateConnID(), routerPacket.getClientConnID(), routerPacket.getBody(), routerPacket.getBodySize());
}

void LogicServer::onAccept(long long clientID)
{

}

void LogicServer::onDisconnect(long long clientID)
{

}

void LogicServer::update(int dt)
{

}

void LogicServer::onHandleMsg(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size)
{

}

void LogicServer::sendToRouter(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size)
{
	RouterProtoPacket routerPacket;
	routerPacket.packing(gateID, clientID, data, size);

	uint16_t packetSize = routerPacket.getSize();
	packetSize = htons(packetSize);
	uint32_t gateConnID = routerPacket.getGateConnID();
	gateConnID = htonl(gateConnID);
	uint32_t clientConnID = routerPacket.getClientConnID();
	clientConnID = htonl(clientConnID);

	send(routerID, (char*)&packetSize, RouterProtoPacket::PACKET_SIZE_BYTES);
	send(routerID, (char*)&gateConnID, RouterProtoPacket::PACKET_GATE_ID_BYTES);
	send(routerID, (char*)&clientConnID, RouterProtoPacket::PACKET_CLIENT_ID_BYTES);
	send(routerID, data, size);
}