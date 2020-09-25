#ifndef __ECHO_SERVER_H__
#define __ECHO_SERVER_H__
#include "../../src/TcpServer.h"
#include "LuaState.h"

class EchoServer : public libnetwork::TcpServer
{
public:
	// �������ݽӿ�
	virtual int onRecv(long long clientID, const char* buf, int size);

	// ���յ��µĿͻ�������
	virtual void onAccept(long long clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(long long clientID);

	// ����
	virtual void update(int dt);
};


#endif // !__ECHO_SERVER_H__
