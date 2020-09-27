#include "GateServer.h"
#include "../../src/Packet.h"
#include "../../proto/GateProtoPacket.h"
#include <arpa/inet.h>
#include "ZookeeperClient.h"
using namespace libnetwork;
const char* CURRENT_SERVICE_NAME = "gate_server";
const char* CURRENT_SERVICE_PARENT_PATH = "/gate";
const char* SERVICE_ROUTER_NAME = "/router";
const char* ZOOKEEPER_HOST = "zk.xxx.com:2181";
const char* CURRENT_SERVER_ADDR_INFO = "xxx.xxx.com:8888";

GateServer::RouterServiceDiscoveryListenner::RouterServiceDiscoveryListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target)
	: ServiceDiscoveryListenner(zkClient, servicePath, target)
{

}

void GateServer::RouterServiceDiscoveryListenner::notify(const std::list<std::string>& serviceInfoArray, void* target)
{
	// 连接路由服务
	GateServer* self = (GateServer*)target;
	for (auto itr = serviceInfoArray.begin(); itr != serviceInfoArray.end(); ++itr)
	{
		const std::string hostAddr = *itr;
		ConnID connID = self->connect(hostAddr.c_str());
		if (connID != INVALID_CONN)
		{
			// 连接成功
			printf("success connectio to router server:%s\n", hostAddr.c_str());
			self->_routerVec.push_back(std::pair<ConnID, std::string>(connID, hostAddr));
		}
		else
		{
			// 连接失败
			printf("failed connect to router server:%s\n", hostAddr.c_str());
		}
	}
}


bool GateServer::onInit()
{
	_zkClient = new ZookeeperClient(ZOOKEEPER_HOST, 5);
	_zkClient->setRegisterServiceName(CURRENT_SERVICE_NAME, CURRENT_SERVICE_PARENT_PATH);
	_zkClient->setRegisterServiceAddrInfo(CURRENT_SERVER_ADDR_INFO);
	_zkClient->addServiceDiscoveryListenner(new RouterServiceDiscoveryListenner(_zkClient, SERVICE_ROUTER_NAME, this));
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

void GateServer::onAccept(ConnID connID)
{

}

void GateServer::onDisconnect(ConnID connID)
{
	// 移除路由服务连接
	removeRouterConnID(connID);
}

void GateServer::update(int dt)
{

}

ConnID GateServer::findRouterServiceID(ConnID clientID)
{
	const int size = _routerVec.size();
	if (size != 0)
	{
		const int index = clientID % size;
		return _routerVec[index].first;
	}
	else
	{
		printf("no router service find.\n");
		return INVALID_CONN;
	}
}

void GateServer::removeRouterConnID(ConnID connID)
{
	for (auto itr = _routerVec.begin(); itr != _routerVec.end(); ++itr)
	{
		if (itr->first == connID)
		{
			_routerVec.erase(itr);
			break;
		}
	}
}