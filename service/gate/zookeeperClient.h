#ifndef __ZOOKEEPER_CLIENT_H__
#define __ZOOKEEPER_CLIENT_H__
#include "ZookeeperWatcher.h"
#include <list>
/**
 * ʵ�ַ���ע�ᣬ�����ֹ���
 */
class ZookeeperClient : public ZookeeperWatcher
{
public:
	// �����б�ı�ص�
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
	// ע�����
	bool registerService();

	// ��ȡ�����б�
	void getServiceList(const std::string& path);

private:
	std::string _path;		// ����·��
	void* _target;
	ServiceListChangeCallback _serverListChangeCb;
	std::list<std::string> _watchList;
};


#endif // !__ZOOKEEPER_CLIENT_H__
