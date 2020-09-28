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
	// ���ݰ�
	virtual void onPacket(ConnID connID, const Packet& packet);

	// ���յ��µĿͻ�������
	virtual void onAccept(ConnID clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(ConnID clientID);

	// ����
	virtual void update(int dt);

public:

	// �������ݵ����ط�
	void sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize);

	// ������Ϣ
	void handleMessage(ConnID gateID, ConnID clientID, const char* data, int dataSize);


private:
	LuaState* _luaState;
};

#endif // !__LOGIC_SERVER_H__
