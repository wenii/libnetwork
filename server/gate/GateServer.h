#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include "ServiceDiscoveryListenner.h"
using namespace libnetwork;
class ZookeeperClient;

class GateServer : public TcpServer
{
public:
	// 初始化
	virtual bool onInit();

	// 数据包
	virtual void onPacket(ConnID connID, const Packet& packet);
	virtual void onPacketFromServer(ConnID connID, const Packet& packet);

	// 接收到新的客户端连接
	virtual void onAccept(ConnID connID);

	// 客户端断开连接
	virtual void onDisconnect(ConnID connID);

	// 更新
	virtual void update(int dt);

public:
	// 路由服务监听器
	class LogicServerListenner : public ServiceDiscoveryListenner
	{
	public:
		LogicServerListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target);
	public:
		virtual void notify(const std::list<std::string>& serviceInfoArray, void* target);
	};

private:
	// 获取逻辑服连接
	ConnID findLogicServerID(ConnID clientID);

	// 移除逻辑服连接
	void removeLogicServerConnID(ConnID connID);

private:
	ZookeeperClient* _zkClient;
	std::vector<std::pair<ConnID, std::string>> _logicServerIDVec;
	std::unordered_map<std::string, std::string> _configMap;
	
};


#endif // !__GATE_SERVER_H__
