#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
namespace libnetwork
{
	class Socket
	{
	public:
		// 构造函数
		Socket();

		// 析构函数
		~Socket();

	public:
		// 建立连接
		bool connect(const char* host, const char* serv);

		// 监听
		bool listen(const char* host, const char* serv);	

	private:
		int _sockfd;		// socket 描述符  
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
