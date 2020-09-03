#ifndef __LIB_NETWORK_CLIENT_H__
#define __LIB_NETWORK_CLIENT_H__

namespace libnetwork
{
	class Connection;
	class EventLoop;
	class Client
	{
	public:
		static const int CLIENT_RUN_HZ_DEFAULT = 60;
		static const int HOST_LEN_MAX = 1024;
		static const int PORT_LEN_MAX = 8;

	public:
		// ÿ֡����
		virtual void update();

		// ���ӳɹ�
		virtual void onConnectSuccess();

		// ����ʧ��
		virtual void onConnectFailed();

		// ���յ�����
		virtual void onRecv(const char* buffer, int size);

		// �Ͽ������˵�����
		virtual void onDisconnect();
		
	public:
		// ���ӷ�����
		void connect(const char* host, const char* port);

		// ��������
		bool send(const char* buffer, int size);

		// �Ͽ�����
		void disconnect();

	private:
		// �����¼������������������̣߳�
		void networkHandler(const char* host, const char* port);

	private:
		// �ͻ���ʱ���¼���ÿ�����hz��
		static int clientCron(struct EventLoop* eventLoop, long long id, void* clientData);

		// ʱ���¼�����
		static void timeEventFinalizerHandler(struct EventLoop* eventLoop, long long id, void* clientData);

		// ���¼�������
		static void readHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask);

		// д�¼�������
		static void writeHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask);

	private:
		// ����Ƿ�Ͽ�����
		void checkDisconnect();

	public:
		// ���캯��
		Client();

		// ��������
		~Client();
	private:
		EventLoop* _eventLoop;									// �¼�ѭ��
		Connection* _serverConn;								// ���������
		char _host[HOST_LEN_MAX];								// ��¼�����ַ
		char _port[PORT_LEN_MAX];								// ��¼����˿�
		int _hz;												// �ͻ�������Ƶ��
		bool isRuning;											// �Ƿ�������
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_CLIENT_H__
