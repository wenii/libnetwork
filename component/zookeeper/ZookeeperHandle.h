#ifndef __ZOOKEEPER_HANDLE_H__
#define __ZOOKEEPER_HANDLE_H__
#include <string>
#include <list>
typedef struct _zhandle zhandle_t;

class ZookeeperWatcher;
class ZookeeperHandle
{

public:
	/**
		* 建立连接
		*/
	bool connect(const std::string& host, unsigned int timeout, ZookeeperWatcher* watcher);

	/**
		* 创建节点
		*/
	bool createEphemeralZNode(const std::string& path, const std::string& value, std::string& newPath);
	bool createZNode(const std::string& path, const std::string& value);

	/**
		* 节点是否存在
		*/
	bool isExist(const std::string& path);

	/**
		* 节点状态watch
		*/
	bool setWatchWhenZnodeCreate(const std::string& path);
	bool setWatchWhenZnodeDelete(const std::string& path);
	bool setWatchWhenZnodeChange(const std::string& path);

	/**
		* 节点数据
		*/
	bool getData(const std::string& path, char* buffer, int* bufferLen, bool watch = false);
	bool setData(const std::string& path, const std::string& data);

	/**
		* 子节点列表
		*/
	bool getChildren(const std::string& path, std::list<std::string>& pathList, bool watch = false);

	/**
		* 是否连接完成状态
		*/
	bool isConnected();


public:
	/**
		* 构造函数
		*/
	ZookeeperHandle();

	/**
		* 析构函数
		*/
	~ZookeeperHandle();

private:
	zhandle_t* _zkHandle;
};


#endif // !__ZOOKEEPER_HANDLE_H__
