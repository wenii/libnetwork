#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
#include <list>
#include <vector>
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
	virtual void onAccept(long long clientID);

	// 客户端断开连接
	virtual void onDisconnect(long long clientID);

	// 更新
	virtual void update(int dt);

public:
	// 服务列表通知
	static void serviceListNotify(const std::string& path, const std::list<std::string>& datas, void* target);

private:
	// 获取路由服务连接
	ConnID findRouterServiceID(ConnID clientID);

private:
	ZookeeperClient* _zkClient;
	std::vector<std::pair<ConnID, std::string>> _routerList;
};


#endif // !__GATE_SERVER_H__
