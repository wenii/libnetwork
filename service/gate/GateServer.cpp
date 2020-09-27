#include "GateServer.h"
#include "../../src/Packet.h"
#include "../../proto/GateProtoPacket.h"
#include <arpa/inet.h>
#include "ZookeeperClient.h"
using namespace libnetwork;

const char* SERVICE_ROUTER_NAME = "/router";
const char* ZOOKEEPER_HOST = "zk.xxx.com:2181";

bool GateServer::onInit()
{
	_zkClient = new ZookeeperClient(ZOOKEEPER_HOST, 5);
	_zkClient->addWatchServicePath(SERVICE_ROUTER_NAME);
	_zkClient->setCallback(serviceListNotify, this);
	return _zkClient->connectToZookeeper();
}

void GateServer::onPacket(ConnID connID, const Packet& packet)
{
	// 修改协议，增加客户端连接ID
	GateProtoPacket gateProtoPacket;
	gateProtoPacket.packing(connID, packet.getBody(), packet.getBodySize());
		
	// 转发包到路由服
	uint16_t packetSize = gateProtoPacket.getSize();
	packetSize = htons(packetSize);
	ConnID routerID = findRouterServiceID(connID);
	if (routerID != INVALID_CONN)
	{
		send(routerID, (char*)&packetSize, sizeof(GateProtoPacket::PACKET_SIZE_BYTES));

		uint32_t clientConnID = gateProtoPacket.getClientConnID();
		clientConnID = htonl(clientConnID);
		send(routerID, (char*)&clientConnID, sizeof(GateProtoPacket::PACKET_CLIENT_ID_BYTES));

		send(routerID, gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
	}
}

void GateServer::onPacketFromServer(ConnID connID, const Packet& packet)
{
	// 从路由服务来的消息
	GateProtoPacket gateProtoPacket;
	gateProtoPacket.unPacking(packet.getSize(), packet.getBody(), packet.getBodySize());

	// 转发给客户端
	const ConnID clientConnID = gateProtoPacket.getClientConnID();
	uint16_t packetSize = gateProtoPacket.getSize() - GateProtoPacket::PACKET_CLIENT_ID_BYTES;
	packetSize = htons(packetSize);
	send(clientConnID, (char*)&packetSize, sizeof(Packet::PACKET_SIZE_BYTES));
	send(clientConnID, gateProtoPacket.getBody(), gateProtoPacket.getBodySize());
}

void GateServer::onAccept(long long clientID)
{

}

void GateServer::onDisconnect(long long clientID)
{

}

void GateServer::update(int dt)
{

}

void GateServer::serviceListNotify(const std::string& path, const std::list<std::string>& datas, void* target)
{
	if (path == SERVICE_ROUTER_NAME)
	{
		// 连接路由服务
		GateServer* self = (GateServer*)target;
		for (auto itr = datas.begin(); itr != datas.end(); ++itr)
		{
			const std::string hostAddr = *itr;
			ConnID connID = self->connect(hostAddr.c_str());
			if (connID != INVALID_CONN)
			{
				// 连接成功
				printf("success connectio to router server:%s\n", hostAddr.c_str());
				self->_routerList.push_back(std::pair<ConnID, std::string>(connID, hostAddr));
			}
			else
			{
				// 连接失败
				printf("failed connect to router server:%s\n", hostAddr.c_str());
			}
		}
	}
}

ConnID GateServer::findRouterServiceID(ConnID clientID)
{
	const int size = _routerList.size();
	if (size != 0)
	{
		const int index = clientID % size;
		return _routerList[index].first;
	}
	else
	{
		printf("no router service find.\n");
		return INVALID_CONN;
	}
}