#ifndef __ROUTER_SERVER_H__
#define __ROUTER_SERVER_H__
#include "../../src/TcpServer.h"
using namespace libnetwork;
class RouterServer : public TcpServer
{
public:
	// 数据包
	virtual void onPacket(ConnID connID, const Packet& packet);

	// 接收到新的客户端连接
	virtual void onAccept(long long clientID);

	// 客户端断开连接
	virtual void onDisconnect(long long clientID);

	// 更新
	virtual void update();

public:
	// 发送数据到逻辑服
	void sendToLogicServer(ConnID logicID, ConnID gateID, ConnID clientID, const char* data, int dataSize);

	// 发送数据到网关服
	void sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize);

private:
	// 路由消息
	void router(ConnID gateID, ConnID clientID, const char* data, int dataSize);
};

#endif // !__ROUTER_SERVER_H__
