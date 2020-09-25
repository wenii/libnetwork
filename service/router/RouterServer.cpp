#include "RouterServer.h"
#include "../../proto/GateProtoPacket.h"
#include "../../src/Packet.h"
#include "../../proto/RouterProtoPacket.h"
#include <arpa/inet.h>
#include "LuaState.h"

RouterServer::RouterServer()
{
	_luaState = new LuaState();
	
	_luaState->loadFile("script/main.lua");
}

RouterServer::~RouterServer()
{

}

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

void RouterServer::onAccept(ConnID clientID)
{
	_luaState->onAccept(clientID);
}

void RouterServer::onDisconnect(ConnID clientID)
{
	_luaState->onDisconnect(clientID);

}

void RouterServer::update(int dt)
{
	_luaState->update(dt);

}

void RouterServer::router(ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	// 路由消息到网关或者逻辑服
	_luaState->onRouter(gateID, clientID, data, dataSize);

}

void RouterServer::sendToLogicServer(ConnID logicID, ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	RouterProtoPacket packet;
	packet.packing(gateID, clientID, data, dataSize);

	uint16_t packetSize = packet.getSize();
	uint32_t gateConnID = packet.getGateConnID();
	uint32_t clientConnID = packet.getClientConnID();
	packetSize = htons(packetSize);
	gateConnID = htonl(gateConnID);
	clientConnID = htonl(clientConnID);

	send(logicID, (char*)&packetSize, RouterProtoPacket::PACKET_SIZE_BYTES);
	send(logicID, (char*)&gateConnID, RouterProtoPacket::PACKET_GATE_ID_BYTES);
	send(logicID, (char*)&clientConnID, RouterProtoPacket::PACKET_CLIENT_ID_BYTES);
	send(logicID, data, dataSize);

}

void RouterServer::sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	GateProtoPacket packet;
	packet.packing(clientID, data, dataSize);
	uint16_t packetSize = packet.getSize();
	uint32_t clientConnID = packet.getClientConnID();
	packetSize = htons(packetSize);
	clientConnID = htonl(clientConnID);

	send(gateID, (char*)&packetSize, GateProtoPacket::PACKET_SIZE_BYTES);
	send(gateID, (char*)&clientConnID, GateProtoPacket::PACKET_CLIENT_ID_BYTES);
	send(gateID, data, dataSize);
}