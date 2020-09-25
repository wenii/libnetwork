#include "../../src/TcpServer.h"
using namespace libnetwork;

class LogicServer : public TcpServer
{
public:
	// 数据包
	virtual void onPacket(ConnID connID, const Packet& packet);

	// 接收到新的客户端连接
	virtual void onAccept(long long clientID);

	// 客户端断开连接
	virtual void onDisconnect(long long clientID);

	// 更新
	virtual void update(int dt);

public:
	// 处理消息
	virtual void onHandleMsg(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size);

	// 发送消息
	void sendToRouter(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size);
};