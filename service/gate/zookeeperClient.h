#ifndef __ZOOKEEPER_CLIENT_H__
#define __ZOOKEEPER_CLIENT_H__
#include "ZookeeperWatcher.h"
#include <list>
/**
 * 实现服务注册，服务发现功能
 */
class ZookeeperClient : public ZookeeperWatcher
{
public:
	// 服务列表改变回调
	typedef void (*ServiceListChangeCallback) (const std::string& path, const std::list<std::string>& datas, void* target);

public:
	ZookeeperClient(const std::string& host, unsigned int timeout);
	~ZookeeperClient();

public:
	bool connectToZookeeper();
	void addWatchServicePath(const std::string& path);
	void setCallback(ServiceListChangeCallback serviceListChange, void* target);

public:
	virtual void onConnected();
	virtual void onZnodeDelete(const std::string& path);
	virtual void onChildrenChange(const std::string& path);

private:
	// 注册服务
	bool registerService();

	// 获取服务列表
	void getServiceList(const std::string& path);

private:
	std::string _path;		// 服务路径
	void* _target;
	ServiceListChangeCallback _serverListChangeCb;
	std::list<std::string> _watchList;
};


#endif // !__ZOOKEEPER_CLIENT_H__
