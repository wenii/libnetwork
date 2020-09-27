#include "ZookeeperClient.h"
#include "ZookeeperHandle.h"
#include <algorithm>
#include "ServiceDiscoveryListenner.h"

ZookeeperClient::ZookeeperClient(const std::string& host, unsigned int timeout)
	: ZookeeperWatcher(host, timeout)
{

}

ZookeeperClient::~ZookeeperClient()
{

}

bool ZookeeperClient::connectToZookeeper()
{
	return connect() != nullptr;
}


void ZookeeperClient::addServiceDiscoveryListenner(ServiceDiscoveryListenner* listenner)
{
	_serviceDiscoveryListennerList[listenner->getPath()] = listenner;
}

void ZookeeperClient::removeServiceDiscoveryListenner(const std::string& path)
{
	auto itr = _serviceDiscoveryListennerList.find(path);
	if (itr != _serviceDiscoveryListennerList.end())
	{
		_serviceDiscoveryListennerList.erase(itr);
		delete itr->second;

		// 移除对服务的观察
		std::list<std::string> childrenPathNameArray;
		ZookeeperHandle* zkHandle = getZkHandle();
		zkHandle->getChildren(path, childrenPathNameArray, false);
	}
}

void ZookeeperClient::setRegisterServiceName(const std::string& serviceName, const std::string& parentPath)
{
	_serviceName = serviceName;
	_parentPath = _parentPath;
}

void ZookeeperClient::setRegisterServiceAddrInfo(const std::string& serviceAddrInfo)
{
	_serviceAddrInfo = serviceAddrInfo;
}

void ZookeeperClient::onConnected()
{
	// 注册服务
	ZookeeperHandle* zkHandle = getZkHandle();
	if ((_servicePath.empty() || !zkHandle->isExist(_servicePath)) && !_serviceName.empty())
	{
		if (registerService())
		{
			printf("success regiseter service when node be deleted!\n");

			// 设置服务地址信息
			if (zkHandle->setData(_servicePath, _serviceAddrInfo))
			{
				printf("success set service addr info.\n");
			}
			else
			{
				printf("failed set service addr info.\n");
			}
		}
		else
		{
			printf("failed register service when node be deleted!\n");
		}
	}

	// 获取服务列表
	for (auto itr = _serviceDiscoveryListennerList.begin(); itr != _serviceDiscoveryListennerList.end(); ++itr)
	{
		ServiceDiscoveryListenner* listenner = itr->second;
		listenner->getService();
	}
}

void ZookeeperClient::onZnodeDelete(const std::string& path)
{
	// 注册服务
	if (_servicePath == path)
	{
		if (registerService()) 
		{
			printf("success regiseter service when node be deleted!\n");
		}
		else
		{
			printf("failed register service when node be deleted!\n");
		}
	}
}

void ZookeeperClient::onChildrenChange(const std::string& path)
{
	getService(path);
}

bool ZookeeperClient::registerService()
{
	ZookeeperHandle* zkHandle = getZkHandle();
	if (!zkHandle->isExist(_parentPath))
	{
		// 创建父路径
		if (!zkHandle->createZNode(_parentPath, ""))
		{
			printf("create parent path failed.\n");
			return false;
		}
	}

	std::string path = _parentPath;
	path.append("/").append(_serviceName);
	_servicePath.clear();
	if (zkHandle->createEphemeralZNode(path, "", _servicePath))
	{
		printf("create node success. path:%s\n", _servicePath.c_str());
		if (zkHandle->setWatchWhenZnodeDelete(_servicePath))
		{
			return true;
		}
		else
		{
			printf("set path:%s watch failed.\n", _servicePath.c_str());
		}
	}
	else
	{
		printf("create node failed.\n");
	}
	return false;
}

void ZookeeperClient::getService(const std::string& path)
{
	auto itr = _serviceDiscoveryListennerList.find(path);
	if (itr != _serviceDiscoveryListennerList.end())
	{
		itr->second->getService();
	}
}