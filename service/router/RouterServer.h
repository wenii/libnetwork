#ifndef __ROUTER_SERVER_H__
#define __ROUTER_SERVER_H__
#include "../../src/TcpServer.h"
using namespace libnetwork;
class LuaState;
class RouterServer : public TcpServer
{
public:
	RouterServer();
	virtual ~RouterServer();
public:
	// ���ݰ�
	virtual void onPacket(ConnID connID, const Packet& packet);

	// ���յ��µĿͻ�������
	virtual void onAccept(ConnID clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(ConnID clientID);

	// ����
	virtual void update(int dt);

public:
	// �������ݵ��߼���
	void sendToLogicServer(ConnID logicID, ConnID gateID, ConnID clientID, const char* data, int dataSize);

	// �������ݵ����ط�
	void sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize);

private:
	// ·����Ϣ
	void router(ConnID gateID, ConnID clientID, const char* data, int dataSize);

private:
	LuaState* _luaState;
};

#endif // !__ROUTER_SERVER_H__
