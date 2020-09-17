#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
class GateServer : public libnetwork::TcpServer
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
};


#endif // !__GATE_SERVER_H__
