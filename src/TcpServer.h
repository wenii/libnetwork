#ifndef __LIB_NETWORK_TCP_SERVER_H__
#define __LIB_NETWORK_TCP_SERVER_H__
#include "Types.h"
namespace libnetwork
{
	class EventLoop;
	class Connection;
	class ConnectionDict;
	class Timer;
	class TimingWheel;
	class Packet;
	class TcpServer
	{
	public:
		static const int BINDADDR_MAX = 16;
		static const int TCP_BACK_LOG_DEFAULT = 128;
		static const int SEVER_RUN_HZ_DEFAULT = 10;
		static const int LISTEN_PORT_DEFAULT = 9042;
		static const int CONFIG_MAX_LINE = 1024;
		static const int CLIENT_LIST_COUNT = 1024;
		static const int CONNECT_TIMEOUT_DEFAULT = 10;

	public:
		// ��������
		void start();

		// ���ӷ���
		ConnID connect(const char* host, const char* port);
		ConnID connect(const char* hostPair);

	public:
		// ��ʼ��
		virtual bool onInit();

		// �������ݽӿ�
		// ����ֵ ���ش��������ֽ���
		virtual int onRecv(ConnID connID, const char* buf, int size);
		virtual int onRecvFromServer(ConnID connID, const char* buf, int size);

		// ���ݰ�
		virtual void onPacket(ConnID connID, const Packet& packet);
		virtual void onPacketFromServer(ConnID connID, const Packet& packet);

		// ���յ��µĿͻ�������
		virtual void onAccept(ConnID connID);

		// ���ӶϿ�
		virtual void onDisconnect(ConnID connID);

		// ����
		virtual void update(int dt);

		// �����˳�
		virtual void onExit();

	public:
		// ��ͻ������ӷ���Data
		void send(ConnID clientID, const char* buffer, int size);

		// �ر�����
		void disconnect(ConnID clientID);

		// ��Ӷ�ʱ��
		void addTimer(Timer* timer);

	public:
		// ��ȡʱ������룩
		static int getSecondTime();

		// ��ȡʱ��������룩
		static long long getMillisTiime();
	
	private:
		// ���տͻ������ӻص�
		static void acceptHandler(int listenFD, void* clientData);

		// �����������
		static void recvCompleteHandler(Connection* conn, void* target);

		// �Ͽ�����
		static void disconnectHandler(Connection* conn, void* target);

		// ʱ���¼��ص���ÿ�����hz��
		static int serverCron(long long id, void* clientData);

		// ʱ��ʱ��ص���ÿ�����1�Σ�����������ʱ��
		static int timerHandler(long long id, void* clientData);

		// ÿ��˯��֮ǰ����
		static void beforeSleepHandler(void* clientData);

		// �ź��¼�
		static void signalHandler(int sig, void* clientData);

	private:
		// ��������
		void loadConfig(const char* fileName);

		// ����
		bool listen();

		// �������տͻ��������¼�
		bool createAcceptEvent();

		// ���»���ʱ��
		void updateCacheTime();

		// �涨ʱ��������true
		bool runWithPeriod(int millisecond);

		// ����Ͽ�������
		void clearDisconnect();

		// ���ͻ��������Ƿ���
		void checkClientConnectionAlive();
	public:
		// ���캯��
		TcpServer();

		// ��������
		virtual ~TcpServer();

	private:
		static int secondTime;     
		static long long millisTime;
		
	private:
		EventLoop* _eventLoop;					// �¼�ѭ��
		char* _bindAddr[BINDADDR_MAX];			// ��ַ����
		int _bindaAddrCount;					// ʵ�ʵ�ַ����
		int _ipFD[BINDADDR_MAX];				// �����ļ�������
		int _ipFDCount;							// ʵ�ʼ����ļ�����������
		int _port;								// �����˿�
		int _backLog;							// ���Ӷ��д�С
		int _hz;								// ����������Ƶ��
		int _timeout;							// ����ͻ�������������ʱ�䣨�룩
		int _cronLoops;							// ��¼serverCron�����ô���
		long long _recvBytes;					// �ѽ����ֽ���
		long long _sendBytes;					// �ѷ����ֽ���
		ConnectionDict* _conns;					// ���л����
		Connection* _connsToRemoveHead;			// �ȴ��Ƴ�������ͷ
		unsigned int _checkClientAliveIndex;	// ���ͻ��˴������
		TimingWheel* _timingWheel;				// ʱ���ֶ�ʱ��
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TCP_SERVER_H__
