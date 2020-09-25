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
		// 启动服务
		void start();

		// 连接服务
		ConnID connect(const char* host, const char* port);
		ConnID connect(const char* hostPair);

	public:
		// 初始化
		virtual bool onInit();

		// 接收数据接口
		// 返回值 返回处理数据字节数
		virtual int onRecv(ConnID connID, const char* buf, int size);
		virtual int onRecvFromServer(ConnID connID, const char* buf, int size);

		// 数据包
		virtual void onPacket(ConnID connID, const Packet& packet);
		virtual void onPacketFromServer(ConnID connID, const Packet& packet);

		// 接收到新的客户端连接
		virtual void onAccept(ConnID connID);

		// 连接断开
		virtual void onDisconnect(ConnID connID);

		// 更新
		virtual void update(int dt);

		// 服务退出
		virtual void onExit();

	public:
		// 向客户端连接发送Data
		void send(ConnID clientID, const char* buffer, int size);

		// 关闭连接
		void disconnect(ConnID clientID);

		// 添加定时器
		void addTimer(Timer* timer);

	public:
		// 获取时间戳（秒）
		static int getSecondTime();

		// 获取时间戳（毫秒）
		static long long getMillisTiime();
	
	private:
		// 接收客户端连接回调
		static void acceptHandler(int listenFD, void* clientData);

		// 接收数据完成
		static void recvCompleteHandler(Connection* conn, void* target);

		// 断开连接
		static void disconnectHandler(Connection* conn, void* target);

		// 时间事件回调，每秒调用hz次
		static int serverCron(long long id, void* clientData);

		// 时间时间回调，每秒调用1次，用于驱动定时器
		static int timerHandler(long long id, void* clientData);

		// 每次睡眠之前调用
		static void beforeSleepHandler(void* clientData);

		// 信号事件
		static void signalHandler(int sig, void* clientData);

	private:
		// 加载配置
		void loadConfig(const char* fileName);

		// 监听
		bool listen();

		// 创建接收客户端连接事件
		bool createAcceptEvent();

		// 更新缓存时间
		void updateCacheTime();

		// 规定时间间隔返回true
		bool runWithPeriod(int millisecond);

		// 清理断开的连接
		void clearDisconnect();

		// 检查客户端连接是否存活
		void checkClientConnectionAlive();
	public:
		// 构造函数
		TcpServer();

		// 析构函数
		virtual ~TcpServer();

	private:
		static int secondTime;     
		static long long millisTime;
		
	private:
		EventLoop* _eventLoop;					// 事件循环
		char* _bindAddr[BINDADDR_MAX];			// 地址数组
		int _bindaAddrCount;					// 实际地址数量
		int _ipFD[BINDADDR_MAX];				// 监听文件描述符
		int _ipFDCount;							// 实际监听文件描述符个数
		int _port;								// 监听端口
		int _backLog;							// 连接队列大小
		int _hz;								// 服务器运行频率
		int _timeout;							// 允许客户端连接最大空闲时间（秒）
		int _cronLoops;							// 记录serverCron被调用次数
		long long _recvBytes;					// 已接收字节数
		long long _sendBytes;					// 已发送字节数
		ConnectionDict* _conns;					// 所有活动连接
		Connection* _connsToRemoveHead;			// 等待移除的连接头
		unsigned int _checkClientAliveIndex;	// 检查客户端存活索引
		TimingWheel* _timingWheel;				// 时间轮定时器
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_TCP_SERVER_H__
