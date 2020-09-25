#include "../../src/TcpServer.h"
using namespace libnetwork;

class LogicServer : public TcpServer
{
public:
	// ���ݰ�
	virtual void onPacket(ConnID connID, const Packet& packet);

	// ���յ��µĿͻ�������
	virtual void onAccept(long long clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(long long clientID);

	// ����
	virtual void update(int dt);

public:
	// ������Ϣ
	virtual void onHandleMsg(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size);

	// ������Ϣ
	void sendToRouter(ConnID routerID, ConnID gateID, ConnID clientID, const char* data, int size);
};