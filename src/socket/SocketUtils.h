#ifndef __LIB_NETWORK_SOCKET_UTILS_H__
#define __LIB_NETWORK_SOCKET_UTILS_H__
namespace libnetwork
{
	class SocketUtils
	{
	public:
		// 获取socket本地地址 xxx.xxx.xxx.xxx:xxx
		static char* getSockLocalAddrInfo(int sockfd);

		// 获取socketd对端地址信息  xxx.xxx.xxx.xxx:xxx
		static char* getSocketPeerAddrInfo(int sockfd);

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_UTILS_H__
