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
			CONNECT_STATE_ESTABLISHED,		// 建立连接
			CONNECT_STATE_CLOSED,			// 连接关闭
		};

		enum 
		{
			CONNECT_TYPE_NONE,
			CONNECT_TYPE_CLIENT,			// 客户端连接
			CONNECT_TYPE_SERVER,			// 服务端连接
		};

		typedef void RecvCompleteCallback(Connection* conn, void* target);
		typedef void DisconnectCallback(Connection* conn,  void* target);

		// 对象池类
		class ObjectPool
		{
			friend class Connection;

			// 最大缓存对象个数
			static const int MAX_OBJECT_COUNT = 1000;
		public:
			static Connection* create();
			static void free(Connection* conn, bool destory = false);
		private:
			static Connection* _conns;
			static int _size;
		};

	private:
		// 创建对象
		static Connection* create();

		// 销毁对象
		void destroy() { delete this; }

	public:
		// 设置事件循环
		void setEventLoop(EventLoop* eventLoop);

		// 获取连接ID
		ConnID getConnectID();
		void setConnectID(ConnID connID);

		// 获取fd
		int getFD();
		void setFD(int fd);

		// 设置连接状态
		void setState(int state);
		int getState();

		// 连接类型
		int getConnectType();
		void setConnectType(int type);

		// 最后一次通信时间
		int getLastTime();
		void setLastTime(int time);

		// 设置回调函数
		void setRecvCompleteCallback(RecvCompleteCallback* callback);
		void setDisconnectCallback(DisconnectCallback* callback);
		void setTarget(void* target);

		// 建立连接
		void established();

		// 关闭连接
		void close();

		// 读数据
		int readFromTcpBuffer();

		// 写数据
		int writeToTcpBuffer();

		// 获取读缓存
		RingBuffer* getReadBuffer();

		// 获取写缓存
		RingBuffer* getWriteBuffer();

		// 设置可读事件
		bool enableRead();

		// 发送数据
		void send(const char* buf, int size);

	public:
		// 读事件回调
		static void readHandler(int fd, void* clientData);

		// 写事件
		static void writeHandler(int fd, void* clientData);

	private:

		// 清理buffer
		void clearBuffer();

	private:
		// 构造函数
		Connection();

		// 析构函数
		~Connection();

	private:
		EventLoop* _eventLoop;		// 事件循环
		ConnID _connectID;			// 客户端ID（递增）
		int _fd;					// 文件描述符
		RingBuffer* _readBuffer;		// 读buffer
		RingBuffer* _writeBuffer;		// 写buffer
		int _state;					// 连接状态
		int _type;					// 连接类型
		int _lastTime;				// 最后一次通信时间
		RecvCompleteCallback* _onRecvComplete;	// 接收数据完成
		DisconnectCallback* _onDisconnect;		// 断开连接回调
		void* _target;					// 回调目标

	public:
		Connection* next;			// 下一个连接
	};

	class ConnectionDict
	{
		static const int RESIZE_RATIO = 5;
	public:
		// 创建对象
		static ConnectionDict* create(int size);

		// 销毁对象
		void destroy();

		// 保存连接
		void saveConnection(Connection* conn);

		// 移除连接
		void removeConnection(Connection* conn);

		// 查找连接
		Connection* findConnectionByID(ConnID connID);

		// 获取连接数量
		int getConnectionCount();

		// 获取连接数组大小
		int getSize();

		// 获取连接数组
		Connection* getConnectionByIndex(unsigned int index);
	private:
		// 扩容
		void dictExpand();

	private:
		// 构造函数
		ConnectionDict(int size);

		// 析构函数
		~ConnectionDict();

	private:
		Connection** _conns;
		int _size;
		int _used;
	};
}	// namespace libnetwork

#endif // !__LIB_NETWORK_CONNECTION_H__
