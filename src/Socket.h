#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
namespace libnetwork
{
	class Socket
	{
	public:
		// 建立连接  
		static int connect(const char* host, const char* port);

		// 监听
		static int listen(const char* addr, const char* port, int proto, int backlog);
		static int listenToPortWithIPv4(const char* addr, const char* port, int backlog);
		static int listenToPortWithIPv6(const char* addr, const char* port, int backlog);

		// 接受连接
		static int accept(int fd, char* addr, int size);

		// 接收数据
		static bool recv(int fd, char* buf, int* size, int flag = 0);

		// 发送数据
		static bool send(int fd, const char* buf, int* size, int flag = 0);

		// 关闭连接
		static void close(int fd);

		// 关闭写
		static void shutdown(int fd);

	private:
		// 获取socket选项
		static bool getSockOpt(int fd, int level, int optname, void* optval, int* optlen);

		// 设置socket选项
		static bool setSockOpt(int fd, int level, int optname, const void* optval, int optlen);

		// 设置不允许IPV6兼容IPV4
		static bool setIPV6Only(int fd);

		// 设置地址可重用
		static bool setReuseAddr(int fd);

		// 设置禁用Nagle算法
		static bool setNodelay(int fd);

		// 设置套接字非阻塞
		static bool setNonBlock(int fd);
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
