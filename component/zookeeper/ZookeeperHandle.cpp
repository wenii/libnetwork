#include "ZookeeperHandle.h"
#include <new>
#include "ZookeeperWatcher.h"
#include "zookeeper.h"

ZookeeperHandle::ZookeeperHandle()
	: _zkHandle(nullptr)
{
}

ZookeeperHandle::~ZookeeperHandle()
{
	if (_zkHandle != nullptr)
	{
		int ret = zookeeper_close(_zkHandle);
		printf("zookeeper closed. ret:%d\n", ret);
	}
}  

bool ZookeeperHandle::connect(const std::string& host, unsigned int timeout, ZookeeperWatcher* watcher)
{
	_zkHandle = zookeeper_init(host.c_str(), ZookeeperWatcher::watcherCallback, timeout, nullptr, watcher, 0);
	return _zkHandle != nullptr;
}

bool ZookeeperHandle::createEphemeralZNode(const std::string& path, const std::string& value, std::string& newPath)
{
	char buffer[256];
	int ret = zoo_create(_zkHandle, path.c_str(), value.c_str(), value.size(), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE, buffer, 256);
	newPath = buffer;

	return ret == ZOK;
}

bool ZookeeperHandle::createZNode(const std::string& path, const std::string& value)
{
	int ret = zoo_create(_zkHandle, path.c_str(), value.c_str(), value.size(), &ZOO_OPEN_ACL_UNSAFE, 0, nullptr, 0);
	return ret == ZOK;
}

bool ZookeeperHandle::isExist(const std::string& path)
{
	struct Stat stat;
	int ret = zoo_exists(_zkHandle, path.c_str(), 0, &stat);
	return ret == ZOK;
}

bool ZookeeperHandle::setWatchWhenZnodeCreate(const std::string& path)
{
	struct Stat stat;
	int ret = zoo_exists(_zkHandle, path.c_str(), ZOO_CREATED_EVENT, &stat);
	return ret == ZOK;
}

bool ZookeeperHandle::setWatchWhenZnodeDelete(const std::string& path)
{
	struct Stat stat;
	int ret = zoo_exists(_zkHandle, path.c_str(), ZOO_DELETED_EVENT, &stat);
	return ret == ZOK; 
}

bool ZookeeperHandle::setWatchWhenZnodeChange(const std::string& path)
{
	struct Stat stat;
	int ret = zoo_exists(_zkHandle, path.c_str(), ZOO_CHANGED_EVENT, &stat);
	return ret == ZOK;
}

bool ZookeeperHandle::getData(const std::string& path, char* buffer, int* bufferLen, bool watch)
{
	struct Stat stat;
	int watchEvent = watch ? ZOO_CHANGED_EVENT : 0;
	int ret = zoo_get(_zkHandle, path.c_str(), watchEvent, buffer, bufferLen, &stat);
	return ret == ZOK;
}

bool ZookeeperHandle::setData(const std::string& path, const std::string& data)
{
	int ret = zoo_set(_zkHandle, path.c_str(), data.c_str(), data.size(), -1);
	return ret == ZOK;
}

bool ZookeeperHandle::getChildren(const std::string& path, std::list<std::string>& pathList, bool watch)
{
	int watchEvent = watch ? ZOO_CHILD_EVENT : 0;
	struct String_vector strings;
	int ret = zoo_get_children(_zkHandle, path.c_str(), watchEvent, &strings);
	for (int i = 0; i < strings.count; ++i)
	{
		pathList.push_back(strings.data[i]);
	}
	return ret == ZOK;
}

bool ZookeeperHandle::isConnected()
{
	return ZOO_CONNECTED_STATE == zoo_state(_zkHandle);
}