#ifndef __TEST_SERVER_H__
#define __TEST_SERVER_H__
#include "../src/TcpServer.h"
namespace libnetwork
{
	class TestServer : public TcpServer
	{
	public:
		TestServer();
	public:
		// 接收数据接口
		virtual int onRecv(long long clientID, const char* buf, int size);

		// 接收到新的客户端连接
		virtual void onAccept(long long clientID);

		// 客户端断开连接
		virtual void onDisconnect(long long clientID);

		// 更新
		virtual void update();
	};
}


#endif // !__TEST_SERVER_H__
