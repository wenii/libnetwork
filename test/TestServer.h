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
		// �������ݽӿ�
		virtual int onRecv(long long clientID, const char* buf, int size);

		// ���յ��µĿͻ�������
		virtual void onAccept(long long clientID);

		// �ͻ��˶Ͽ�����
		virtual void onDisconnect(long long clientID);

		// ����
		virtual void update();
	};
}


#endif // !__TEST_SERVER_H__
