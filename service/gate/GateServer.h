#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "../../src/TcpServer.h"
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include "ServiceDiscoveryListenner.h"
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
	virtual void onAccept(ConnID connID);

	// �ͻ��˶Ͽ�����
	virtual void onDisconnect(ConnID connID);

	// ����
	virtual void update(int dt);

public:
	// ·�ɷ��������
	class RouterServiceDiscoveryListenner : public ServiceDiscoveryListenner
	{
	public:
		RouterServiceDiscoveryListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target);
	public:
		virtual void notify(const std::list<std::string>& serviceInfoArray, void* target);
	};

private:
	// ��ȡ·�ɷ�������
	ConnID findRouterServiceID(ConnID clientID);

	// �Ƴ�·�ɷ�������
	void removeRouterConnID(ConnID connID);

private:
	ZookeeperClient* _zkClient;
	std::vector<std::pair<ConnID, std::string>> _routerVec;
	std::unordered_map<std::string, std::string> _configMap;
	
};


#endif // !__GATE_SERVER_H__
