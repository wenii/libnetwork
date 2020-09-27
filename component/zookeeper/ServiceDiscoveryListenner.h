#ifndef __SERVICE_DISCOVERY_LISTENNER_H__
#define __SERVICE_DISCOVERY_LISTENNER_H__
#include <string>
#include <list>
class ZookeeperClient;

// 服务发现监听器
class ServiceDiscoveryListenner
{
public:
	ServiceDiscoveryListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target);
	virtual ~ServiceDiscoveryListenner();
public:
	virtual void notify(const std::list<std::string>& serviceInfoArray, void* target) = 0;

public:
	void getService();
	const std::string& getPath() const;

private:
	bool isNewService(const std::string& serviceInfo);

private:
	ZookeeperClient* _zkClient;
	std::string _servicePath;
	void* _target;
	std::list<std::string> _serviceList;
};


#endif // !__SERVICE_DISCOVERY_LISTENNER_H__
