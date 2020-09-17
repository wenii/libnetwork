#ifndef __ROUTER_SERVER_H__
#define __ROUTER_SERVER_H__
#include "../../src/TcpServer.h"
using namespace libnetwork;
class RouterServer : public TcpServer
{
public:
	// ���ݰ�
	virtual void onPacket(ConnID connID, const Packet& packet);

	// ���յ��µĿͻ�������
	virtual void onAccept(long long clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(long long clientID);

	// ����
	virtual void update();

public:
	// �������ݵ��߼���
	void sendToLogicServer(ConnID logicID, ConnID gateID, ConnID clientID, const char* data, int dataSize);

	// �������ݵ����ط�
	void sendToGateServer(ConnID gateID, ConnID clientID, const char* data, int dataSize);

private:
	// ·����Ϣ
	void router(ConnID gateID, ConnID clientID, const char* data, int dataSize);
};

#endif // !__ROUTER_SERVER_H__
