#ifndef __ZOOKEEPER_CLIENT_H__
#define __ZOOKEEPER_CLIENT_H__
#include "ZookeeperWatcher.h"
#include <list>
#include <unordered_map>

/**
 * 实现服务注册，服务发现功能
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
	// 注册服务
	bool registerService();

	// 获取服务列表
	void getService(const std::string& path);

private:
	std::string _servicePath;		// 已注册服务路径
	std::string _serviceName;		// 服务名称
	std::string _parentPath;		// 服务父路径
	std::string _serviceAddrInfo;	// 服务地址信息
	std::unordered_map<std::string, ServiceDiscoveryListenner*> _serviceDiscoveryListennerList;
};


#endif // !__ZOOKEEPER_CLIENT_H__
