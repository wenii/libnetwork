#ifndef __LIB_NETWORK_CONNECTION_H__
#define __LIB_NETWORK_CONNECTION_H__
#include "Types.h"
#include <functional>
namespace libnetwork
{
	class RingBuffer;
	class EventLoop;
	class Connection
	{
	public:
		enum 
		{
			CONNECT_STATE_NONE,
			CONNECT_STATE_ESTABLISHED,		// ��������
			CONNECT_STATE_CLOSED,			// ���ӹر�
		};

		enum 
		{
			CONNECT_TYPE_NONE,
			CONNECT_TYPE_CLIENT,			// �ͻ�������
			CONNECT_TYPE_SERVER,			// ���������
		};

		typedef void RecvCompleteCallback(Connection* conn, void* target);
		typedef void DisconnectCallback(Connection* conn,  void* target);

		// �������
		class ObjectPool
		{
			friend class Connection;

			// ��󻺴�������
			static const int MAX_OBJECT_COUNT = 1000;
		public:
			static Connection* create();
			static void free(Connection* conn, bool destory = false);
		private:
			static Connection* _conns;
			static int _size;
		};

	private:
		// ��������
		static Connection* create();

		// ���ٶ���
		void destroy() { delete this; }

	public:
		// �����¼�ѭ��
		void setEventLoop(EventLoop* eventLoop);

		// ��ȡ����ID
		ConnID getConnectID();
		void setConnectID(ConnID connID);

		// ��ȡfd
		int getFD();
		void setFD(int fd);

		// ��������״̬
		void setState(int state);
		int getState();

		// ��������
		int getConnectType();
		void setConnectType(int type);

		// ���һ��ͨ��ʱ��
		int getLastTime();
		void setLastTime(int time);

		// ���ûص�����
		void setRecvCompleteCallback(RecvCompleteCallback* callback);
		void setDisconnectCallback(DisconnectCallback* callback);
		void setTarget(void* target);

		// ��������
		void established();

		// �ر�����
		void close();

		// ������
		int readFromTcpBuffer();

		// д����
		int writeToTcpBuffer();

		// ��ȡ������
		RingBuffer* getReadBuffer();

		// ��ȡд����
		RingBuffer* getWriteBuffer();

		// ���ÿɶ��¼�
		bool enableRead();

		// ��������
		void send(const char* buf, int size);

	public:
		// ���¼��ص�
		static void readHandler(int fd, void* clientData);

		// д�¼�
		static void writeHandler(int fd, void* clientData);

	private:

		// ����buffer
		void clearBuffer();

	private:
		// ���캯��
		Connection();

		// ��������
		~Connection();

	private:
		EventLoop* _eventLoop;		// �¼�ѭ��
		ConnID _connectID;			// �ͻ���ID��������
		int _fd;					// �ļ�������
		RingBuffer* _readBuffer;		// ��buffer
		RingBuffer* _writeBuffer;		// дbuffer
		int _state;					// ����״̬
		int _type;					// ��������
		int _lastTime;				// ���һ��ͨ��ʱ��
		RecvCompleteCallback* _onRecvComplete;	// �����������
		DisconnectCallback* _onDisconnect;		// �Ͽ����ӻص�
		void* _target;					// �ص�Ŀ��

	public:
		Connection* next;			// ��һ������
	};

	class ConnectionDict
	{
		static const int RESIZE_RATIO = 5;
	public:
		// ��������
		static ConnectionDict* create(int size);

		// ���ٶ���
		void destroy();

		// ��������
		void saveConnection(Connection* conn);

		// �Ƴ�����
		void removeConnection(Connection* conn);

		// ��������
		Connection* findConnectionByID(ConnID connID);

		// ��ȡ��������
		int getConnectionCount();

		// ��ȡ���������С
		int getSize();

		// ��ȡ��������
		Connection* getConnectionByIndex(unsigned int index);
	private:
		// ����
		void dictExpand();

	private:
		// ���캯��
		ConnectionDict(int size);

		// ��������
		~ConnectionDict();

	private:
		Connection** _conns;
		int _size;
		int _used;
	};
}	// namespace libnetwork

#endif // !__LIB_NETWORK_CONNECTION_H__
