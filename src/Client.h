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
		// 每帧更新
		virtual void update();

		// 连接成功
		virtual void onConnectSuccess();

		// 连接失败
		virtual void onConnectFailed();

		// 接收到数据
		virtual void onRecv(const char* buffer, int size);

		// 断开与服务端的连接
		virtual void onDisconnect();
		
	public:
		// 连接服务器
		void connect(const char* host, const char* port);

		// 发送数据
		bool send(const char* buffer, int size);

		// 断开连接
		void disconnect();

	private:
		// 网络事件处理器（运行在子线程）
		void networkHandler(const char* host, const char* port);

	private:
		// 客户端时间事件，每秒调用hz次
		static int clientCron(struct EventLoop* eventLoop, long long id, void* clientData);

		// 时间事件结束
		static void timeEventFinalizerHandler(struct EventLoop* eventLoop, long long id, void* clientData);

		// 读事件处理器
		static void readHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask);

		// 写事件处理器
		static void writeHandler(struct EventLoop* eventLoop, int fd, void* clientData, int mask);

	private:
		// 检查是否断开连接
		void checkDisconnect();

	public:
		// 构造函数
		Client();

		// 析构函数
		~Client();
	private:
		EventLoop* _eventLoop;									// 事件循环
		Connection* _serverConn;								// 服务端连接
		char _host[HOST_LEN_MAX];								// 记录服务地址
		char _port[PORT_LEN_MAX];								// 记录服务端口
		int _hz;												// 客户端运行频率
		bool isRuning;											// 是否在运行
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_CLIENT_H__
