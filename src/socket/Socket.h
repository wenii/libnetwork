#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
#include <time.h>
namespace libnetwork
{
	class Socket
	{
	public:
		// 构造函数
		Socket();
		Socket(int sockfd);

		// 析构函数
		~Socket();

	public:
		// socket 可用
		bool isValide() const;

		// 获取socket描述符
		int getSockFD() const;

	public:
		// 设置禁用Nagle算法
		void setTcpNodelay();
		
	public:
		// 建立连接  
		bool connect(const char* host, const char* serv);

		// 监听
		bool listen(const char* host, const char* serv);	

		// 接受连接
		Socket accept();

		// 接收数据
		bool recv(char* buf, int* size, int flag = 0);

		// 发送数据
		bool send(const char* buf, int* size, int flag = 0);

		// 关闭连接
		bool close();

	private:
		// 获取socket选项
		bool getSockOpt(int level, int optname, void* optval, int* optlen);

		// 设置socket选项
		bool setSockOpt(int level, int optname, const void* optval, int optlen);

		// 设置用户自定义socket参数
		void setSockOptCustom();

	private:
		int _sockfd;						// socket 描述符  
		int _nodelay;						// 是否使用Nagle算法	TCP_NODELAY
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
