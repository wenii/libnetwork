#ifndef __ZOOKEEPER_WATCHER_H__
#define __ZOOKEEPER_WATCHER_H__
#include <string>
#include <queue>
typedef struct _zhandle zhandle_t;

class ZookeeperHandle;
class ZookeeperWatcher
{
public:
	class WatchedEvent
	{
	public:
		/**
			* 获取类型
			*/
		int getType() { return _type; }
			
		/**
			* 获取状态
			*/
		int getState() { return _state; }
			
		/**
			* 获取路径
			*/
		const std::string& getPath() { return _path; }

	public:
		/**
			* 构造函数
			*/
		WatchedEvent(int type, int state, const std::string& path);
		

	private:
		int _type;					// 事件类型
		int _state;					// 事件状态
		std::string _path;			// 路径
	};
public:
	/**
		* 构造函数
		*/
	ZookeeperWatcher(const std::string& host, unsigned int timeout);

	/**
		* 析构函数
		*/
	virtual~ZookeeperWatcher();

public:
	/**
		* 连接zookeeper server
		*/
	ZookeeperHandle* connect();

	/**
		* 获取zookeeper handle
		*/
	ZookeeperHandle* getZkHandle();

	/**
		* 通知事件
		*/
	void handleNotify();

public:
	/**
		* 连接状态
		*/
	virtual void onConnecting();
	virtual void onConnected();

	/**
		* session过期
		*/
	virtual void onSessionExpired();

	/**
		* 子节点发生变化
		*/
	virtual void onChildrenChange(const std::string& path);

	/**
		* 节点发生变化
		*/
	virtual void onZnodeChange(const std::string& path);

	/**
		* 节点被创建
		*/
	virtual void onZnodeCreate(const std::string& path);

	/**
		* 节点被删除
		*/
	virtual void onZnodeDelete(const std::string& path);

public:
	/**
		* 回调
		*/
	static void watcherCallback(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

private:
	/**
		* 重连
		*/
	bool reconnect();


	/**
		* 通知事件
		*/
	void onNotify(WatchedEvent* event);

private:
	/**
		* 处理回调
		*/
	static void handleCallback(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);

private:
	/**
		* 获取事件队列
		*/
	std::queue<WatchedEvent*>& getEventQueue(){ return _eventQueue; }

private:
	ZookeeperHandle* _zkHandle;
	std::string _host;
	unsigned int _timeout;
	std::queue<WatchedEvent*> _eventQueue;
};



#endif // !__ZOOKEEPER_WATCHER_H__

