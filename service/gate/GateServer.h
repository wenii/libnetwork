#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
#include <list>
#include <vector>
using namespace libnetwork;
class ZookeeperClient;

class GateServer : public TcpServer
{
public:
	// ��ʼ��
	virtual bool onInit();

	// ���ݰ�
	virtual void onPacket(ConnID connID, const Packet& packet);
	virtual void onPacketFromServer(ConnID connID, const Packet& packet);

	// ���յ��µĿͻ�������
	virtual void onAccept(long long clientID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(long long clientID);

	// ����
	virtual void update(int dt);

public:
	// �����б�֪ͨ
	static void serviceListNotify(const std::string& path, const std::list<std::string>& datas, void* target);

private:
	// ��ȡ·�ɷ�������
	ConnID findRouterServiceID(ConnID clientID);

private:
	ZookeeperClient* _zkClient;
	std::vector<std::pair<ConnID, std::string>> _routerList;
};


#endif // !__GATE_SERVER_H__
