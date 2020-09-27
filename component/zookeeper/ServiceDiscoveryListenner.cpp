#include "ServiceDiscoveryListenner.h"
#include "ZookeeperHandle.h"
#include "ZookeeperClient.h"
ServiceDiscoveryListenner::ServiceDiscoveryListenner(ZookeeperClient* zkClient, const std::string& servicePath, void* target)
	: _zkClient(zkClient)
	, _servicePath(servicePath)
	, _target(target)
{

}

ServiceDiscoveryListenner::~ServiceDiscoveryListenner()
{

}

void ServiceDiscoveryListenner::getService()
{
	// 获取服务信息
	std::list<std::string> serviceInfoArray;
	std::list<std::string> newList;
	ZookeeperHandle* zkHandle = _zkClient->getZkHandle();
	if (zkHandle != nullptr)
	{
		std::list<std::string> childrenPathNameArray;
		zkHandle->getChildren(_servicePath, childrenPathNameArray, true);
		for (auto itr = childrenPathNameArray.begin(); itr != childrenPathNameArray.end(); ++itr)
		{
			const std::string& childrenPath = *itr;
			std::string fullPath = _servicePath + "/" + childrenPath;
			char buffer[1024] = { 0 };
			int size = 1024;
			if (zkHandle->getData(fullPath, buffer, &size))
			{
				if (isNewService(buffer))
				{
					newList.push_back(buffer);
				}
				serviceInfoArray.push_back(buffer);
			}
		}
	}

	// 更新服务列表
	_serviceList = std::move(serviceInfoArray);

	// 通知
	if (newList.size() > 0)
	{
		notify(newList, _target);
	}
}

const std::string& ServiceDiscoveryListenner::getPath() const
{
	return _servicePath;
}

bool ServiceDiscoveryListenner::isNewService(const std::string& serviceInfo)
{
	for (auto itr = _serviceList.begin(); itr != _serviceList.end(); ++itr)
	{
		if (serviceInfo == *itr)
		{
			return false;
		}
	}
	return true;
}