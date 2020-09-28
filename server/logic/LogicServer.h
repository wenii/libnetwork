#ifndef __LOGIC_SERVER_H__
#define __LOGIC_SERVER_H__
#include "../../src/TcpServer.h"
using namespace libnetwork;
class LuaState;
class LogicServer : public TcpServer
{
public:
	LogicServer();
	virtual ~LogicServer();
public:
	// 数据包
	virtual void onPacket(ConnID connID, const Packet& packet);

	// 接收到新的客户端连接
	virtual void onAccept(ConnID clientID);

	// 客户端断开连接
	virtual void onDisconnect(ConnID clientID);

	// 更新
	virtual void update(int dt);

public:

	// 发送数据到网关服
	void sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize);

	// 处理消息
	void handleMessage(ConnID gateID, ConnID clientID, const char* data, int dataSize);


private:
	LuaState* _luaState;
};

#endif // !__LOGIC_SERVER_H__
