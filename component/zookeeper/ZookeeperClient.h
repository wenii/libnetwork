#ifndef __ZOOKEEPER_CLIENT_H__
#define __ZOOKEEPER_CLIENT_H__
#include "ZookeeperWatcher.h"
#include <list>
#include <unordered_map>

/**
 * ʵ�ַ���ע�ᣬ�����ֹ���
 */
class ServiceDiscoveryListenner;
class ZookeeperClient : public ZookeeperWatcher
{
public:
	ZookeeperClient(const std::string& host, unsigned int timeout);
	~ZookeeperClient();

public:
	bool connectToZookeeper();
	void addServiceDiscoveryListenner(ServiceDiscoveryListenner* listenner);
	void removeServiceDiscoveryListenner(const std::string& path);
	void setRegisterServiceName(const std::string& serviceName, const std::string& parentPath = "/");
	void setRegisterServiceAddrInfo(const std::string& serviceAddrInfo);

public:
	virtual void onConnected();
	virtual void onZnodeDelete(const std::string& path);
	virtual void onChildrenChange(const std::string& path);

private:
	// ע�����
	bool registerService();

	// ��ȡ�����б�
	void getService(const std::string& path);

private:
	std::string _servicePath;		// ��ע�����·��
	std::string _serviceName;		// ��������
	std::string _parentPath;		// ����·��
	std::string _serviceAddrInfo;	// �����ַ��Ϣ
	std::unordered_map<std::string, ServiceDiscoveryListenner*> _serviceDiscoveryListennerList;
};


#endif // !__ZOOKEEPER_CLIENT_H__
