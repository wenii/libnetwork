#include "LogicServer.h"
#include "../../proto/GateProtoPacket.h"
#include "../../src/Packet.h"
#include <arpa/inet.h>
#include "LuaState.h"

LogicServer::LogicServer()
{
	_luaState = new LuaState();
	
	_luaState->loadFile("script/main.lua");
}

LogicServer::~LogicServer()
{

}

void LogicServer::onPacket(ConnID connID, const Packet& packet)
{
	GateProtoPacket gateProtoPacket;
	gateProtoPacket.unPacking(packet.getSize(), packet.getBody(), packet.getBodySize());

	handleMessage(connID, gateProtoPacket.getClientConnID(), gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
}

void LogicServer::onAccept(ConnID clientID)
{
	_luaState->onAccept(clientID);
}

void LogicServer::onDisconnect(ConnID clientID)
{
	_luaState->onDisconnect(clientID);

}

void LogicServer::update(int dt)
{
	_luaState->update(dt);

}

void LogicServer::handleMessage(ConnID gateID, ConnID clientID, const char* data, int dataSize)
{
	_luaState->onMessage(gateID, clientID, data, dataSize);
}


void LogicServer::sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize)
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