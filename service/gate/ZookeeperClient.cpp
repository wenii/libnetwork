#include "ZookeeperClient.h"
#include "ZookeeperHandle.h"
#include <algorithm>
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

void ZookeeperClient::addWatchServicePath(const std::string& path)
{
	auto itr = std::find(_watchList.begin(), _watchList.end(), path);
	if (itr == _watchList.end())
	{
		_watchList.push_back(path);
	}
}

void ZookeeperClient::setCallback(ServiceListChangeCallback serviceListChange, void* target)
{
	_serverListChangeCb = serviceListChange;
	_target = target;
}

void ZookeeperClient::onConnected()
{
	// 注册服务
	ZookeeperHandle* zkHandle = getZkHandle();
	if (_path.empty() || !zkHandle->isExist(_path))
	{
		if (registerService())
		{
			printf("success regiseter service when node be deleted!\n");

			// 获取服务列表
			for (auto itr = _watchList.begin(); itr != _watchList.end(); ++itr)
			{
				const std::string path = *itr;
				getServiceList(path);
			}
		}
		else
		{
			printf("failed register service when node be deleted!\n");
		}
	}
}

void ZookeeperClient::onZnodeDelete(const std::string& path)
{
	// 注册服务
	ZookeeperHandle* zkHandle = getZkHandle();
	if (_path == path)
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
	getServiceList(path);
}

bool ZookeeperClient::registerService()
{
	const char* parentPath = "/GateServer";
	const char* serviceName = "gate";
	ZookeeperHandle* zkHandle = getZkHandle();
	if (!zkHandle->isExist(parentPath))
	{
		// 创建父路径
		if (!zkHandle->createZNode(parentPath, ""))
		{
			printf("create parent path failed.\n");
			return false;
		}
	}

	std::string path = parentPath;
	path.append("/").append(serviceName);
	_path.clear();
	if (zkHandle->createEphemeralZNode(path, "", _path))
	{
		printf("create node success. path:%s\n", _path.c_str());
		if (zkHandle->setWatchWhenZnodeDelete(_path))
		{
			return true;
		}
		else
		{
			printf("set path:%s watch failed.\n", _path.c_str());
		}
	}
	else
	{
		printf("create node failed.\n");
	}
	return false;
}

void ZookeeperClient::getServiceList(const std::string& path)
{
	ZookeeperHandle* zkHandle = getZkHandle();
	if (zkHandle != nullptr)
	{
		if (_serverListChangeCb && _target)
		{
			std::list<std::string> datas;
			zkHandle->getChildren(path, datas, true);
			(*_serverListChangeCb)(path, datas, _target);
		}
	}
}
