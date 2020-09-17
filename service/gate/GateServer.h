#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
class GateServer : public libnetwork::TcpServer
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
};


#endif // !__GATE_SERVER_H__
