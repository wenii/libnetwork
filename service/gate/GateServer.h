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
	class RouterServiceDiscoveryListenner : public ServiceDiscoveryListenner
	{
	public:
		RouterServiceDiscoveryListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target);
	public:
		virtual void notify(const std::list<std::string>& serviceInfoArray, void* target);
	};

private:
	// 获取路由服务连接
	ConnID findRouterServiceID(ConnID clientID);

	// 移除路由服务连接
	void removeRouterConnID(ConnID connID);

private:
	ZookeeperClient* _zkClient;
	std::vector<std::pair<ConnID, std::string>> _routerVec;
	std::unordered_map<std::string, std::string> _configMap;
	
};


#endif // !__GATE_SERVER_H__
