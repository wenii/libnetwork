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
	_parentPath = parentPath;
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
			printf("success regiseter service when connect to zookeeper host!\n");
		}
		else
		{
			printf("failed register service when connect to zookeeper host!\n");
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
	// 创建节点
	if (zkHandle->createEphemeralZNode(path, "", _servicePath))
	{
		printf("create node success. path:%s\n", _servicePath.c_str());
		// 设置服务地址信息
		if (zkHandle->setData(_servicePath, _serviceAddrInfo))
		{
			printf("success set service addr info.\n");
			// 设置删除事件，防止外部意外删除节点
			if (zkHandle->setWatchWhenZnodeDelete(_servicePath))
			{
				printf("set path:%s delete event success.\n", _servicePath.c_str());
				return true;
			}
			else
			{
				printf("set path:%s delete event failed.\n", _servicePath.c_str());
			}
		}
		else
		{
			printf("failed set service addr info.\n");
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