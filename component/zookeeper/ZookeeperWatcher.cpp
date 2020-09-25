#include "ZookeeperWatcher.h"
#include "ZookeeperHandle.h"
#include "zookeeper.h"


ZookeeperWatcher::WatchedEvent::WatchedEvent(int type, int state, const std::string& path)
	: _type(0)
	, _state(0)
{
	_type = type;
	_state = state;
	_path = path;
}

ZookeeperWatcher::ZookeeperWatcher(const std::string& host, unsigned int timeout)
	: _zkHandle(nullptr)
	, _host(host)
	, _timeout(timeout)
{
}

ZookeeperWatcher::~ZookeeperWatcher()
{
}

ZookeeperHandle* ZookeeperWatcher::connect()
{
	if (_zkHandle == nullptr)
	{
		_zkHandle = new ZookeeperHandle();
	}
	if (_zkHandle->connect(_host, _timeout, this))
	{
		return _zkHandle;
	}
	delete _zkHandle;
	return nullptr;
}

ZookeeperHandle* ZookeeperWatcher::getZkHandle()
{
	return _zkHandle;
}

void ZookeeperWatcher::onNotify(WatchedEvent* event)
{
	const int type = event->getType();
	const int state = event->getState();
	const std::string& path = event->getPath();
	if (type == ZOO_SESSION_EVENT)
	{
		if (state == ZOO_CONNECTING_STATE)
			onConnecting();
		else if (state == ZOO_CONNECTED_STATE)
			onConnected();
		else if (state == ZOO_EXPIRED_SESSION_STATE)
		{
			onSessionExpired();
			reconnect();
		}
		else
			printf("unhandle state:%d", state);
	}
	else if (type == ZOO_CHILD_EVENT)
		onChildrenChange(path);
	else if (type == ZOO_CREATED_EVENT)
		onZnodeCreate(path);
	else if (type == ZOO_DELETED_EVENT)
		onZnodeDelete(path);
	else if (type == ZOO_CHANGED_EVENT)
		onZnodeChange(path);
	else
		printf("unhandle type:%d", type);
}


void ZookeeperWatcher::handleNotify()
{
	int eventSize = _eventQueue.size();
	if (eventSize > 0)
	{
		std::queue<WatchedEvent*> eventQueue;
		eventQueue.swap(_eventQueue);
		while (eventSize--)
		{
			WatchedEvent*& event = eventQueue.front();
			if (event != nullptr)
			{
				onNotify(event);

				// 销毁事件
				delete event;
			}
			eventQueue.pop();
		}
	}
}

bool ZookeeperWatcher::reconnect()
{
	if (_zkHandle != nullptr)
	{
		delete _zkHandle;
	}
	_zkHandle = new ZookeeperHandle();
	if (_zkHandle->connect(_host, _timeout, this)) 
	{
		return true;
	}
	delete _zkHandle;
	_zkHandle = nullptr;
	return false;
}

void ZookeeperWatcher::onConnecting()
{
}
void ZookeeperWatcher::onConnected()
{
}

void ZookeeperWatcher::onSessionExpired()
{
}

void ZookeeperWatcher::onChildrenChange(const std::string& path)
{
}

void ZookeeperWatcher::onZnodeChange(const std::string& path)
{
}

void ZookeeperWatcher::onZnodeCreate(const std::string& path)
{
}

void ZookeeperWatcher::onZnodeDelete(const std::string& path)
{
}

void ZookeeperWatcher::watcherCallback(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
	ZookeeperWatcher* zkWatcher = (ZookeeperWatcher*)watcherCtx;
	std::queue<WatchedEvent*>& eventQueue = zkWatcher->getEventQueue();
	ZookeeperWatcher::WatchedEvent* watchedEvent = new ZookeeperWatcher::WatchedEvent(type, state, path);
	if (watchedEvent != nullptr)
	{
		eventQueue.push(watchedEvent);
	}
}