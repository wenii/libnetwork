#include "GateServer.h"
#include "../../src/Packet.h"
#include "../../proto/GateProtoPacket.h"
#include <arpa/inet.h>
#include <string.h>
#include "ZookeeperClient.h"
using namespace libnetwork;

static const char* CONFIG_FILE_NAME = "gate.cfg";
static const char* CONFIG_FIELD_ADDR_INFO = "addrInfo";
static const char* CONFIG_FIELD_ZOOKEEPER_HOST = "zookeeperHost";
static const char* CONFIG_FIELD_SERVER_NAME = "serverName";
static const char* CONFIG_FIELD_SERVER_PARENT_PATH = "serverParentPath";
static const char* CONFIG_FIELD_LOGIC_SERVER_PATH = "logicServerPath";
static const char* CONFIG_FIELD_ZOOKEEPER_TIMEOUT = "zkTimeout";


GateServer::LogicServerListenner::LogicServerListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target)
	: ServiceDiscoveryListenner(zkClient, servicePath, target)
{

}

void GateServer::LogicServerListenner::notify(const std::list<std::string>& serviceInfoArray, void* target)
{
	// 连接逻辑服
	GateServer* self = (GateServer*)target;
	for (auto itr = serviceInfoArray.begin(); itr != serviceInfoArray.end(); ++itr)
	{
		const std::string hostAddr = *itr;
		ConnID connID = self->connect(hostAddr.c_str());
		if (connID != INVALID_CONN)
		{
			// 连接成功
			printf("success connectio to logic server:%s\n", hostAddr.c_str());
			self->_logicServerIDVec.push_back(std::pair<ConnID, std::string>(connID, hostAddr));
		}
		else
		{
			// 连接失败
			printf("failed connect to logic server:%s\n", hostAddr.c_str());
		}
	}
}


bool GateServer::onInit()
{
	// 读取配置
	loadConfig(CONFIG_FILE_NAME, [this](const char* field, const char* value) -> bool {
		if (!strcmp(field, CONFIG_FIELD_ADDR_INFO))
			_configMap[CONFIG_FIELD_ADDR_INFO] = value;
		else if (!strcmp(field, CONFIG_FIELD_LOGIC_SERVER_PATH))
			_configMap[CONFIG_FIELD_LOGIC_SERVER_PATH] = value;
		else if (!strcmp(field, CONFIG_FIELD_SERVER_NAME))
			_configMap[CONFIG_FIELD_SERVER_NAME] = value;
		else if (!strcmp(field, CONFIG_FIELD_SERVER_PARENT_PATH))
			_configMap[CONFIG_FIELD_SERVER_PARENT_PATH] = value;
		else if (!strcmp(field, CONFIG_FIELD_ZOOKEEPER_HOST))
			_configMap[CONFIG_FIELD_ZOOKEEPER_HOST] = value;
		else if (!strcmp(field, CONFIG_FIELD_ZOOKEEPER_TIMEOUT))
			_configMap[CONFIG_FIELD_ZOOKEEPER_TIMEOUT] = value;
		else
			return false;
		return true;
		});
	
	// 初始化zookeeper
	const int zkTimeout = atoi(_configMap[CONFIG_FIELD_ZOOKEEPER_TIMEOUT].c_str());
	_zkClient = new ZookeeperClient(_configMap[CONFIG_FIELD_ZOOKEEPER_HOST], zkTimeout);
	_zkClient->setRegisterServiceName(_configMap[CONFIG_FIELD_SERVER_NAME], _configMap[CONFIG_FIELD_SERVER_PARENT_PATH]);
	_zkClient->setRegisterServiceAddrInfo(_configMap[CONFIG_FIELD_ADDR_INFO]);
	_zkClient->addServiceDiscoveryListenner(new LogicServerListenner(_zkClient, _configMap[CONFIG_FIELD_LOGIC_SERVER_PATH], this));
	return _zkClient->connectToZookeeper();
}

void GateServer::onPacket(ConnID connID, const Packet& packet)
{
	// 修改协议，增加客户端连接ID
	GateProtoPacket gateProtoPacket;
	gateProtoPacket.packing(connID, packet.getBody(), packet.getBodySize());
		
	// 转发包到逻辑服
	uint16_t packetSize = gateProtoPacket.getSize();
	packetSize = htons(packetSize);
	ConnID routerID = findLogicServerID(connID);
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
	// 从逻辑服来的消息
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
	// 移除逻辑服连接
	removeLogicServerConnID(connID);
}

void GateServer::update(int dt)
{
	_zkClient->handleNotify();


}

ConnID GateServer::findLogicServerID(ConnID clientID)
{
	const int size = _logicServerIDVec.size();
	if (size != 0)
	{
		const int index = clientID % size;
		return _logicServerIDVec[index].first;
	}
	else
	{
		printf("no logic server find.\n");
		return INVALID_CONN;
	}
}

void GateServer::removeLogicServerConnID(ConnID connID)
{
	for (auto itr = _logicServerIDVec.begin(); itr != _logicServerIDVec.end(); ++itr)
	{
		if (itr->first == connID)
		{
			_logicServerIDVec.erase(itr);
			break;
		}
	}
}