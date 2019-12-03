#ifndef __LIB_NETWORK_SOCKET_UTILS_H__
#define __LIB_NETWORK_SOCKET_UTILS_H__
namespace libnetwork
{
	class SocketUtils
	{
	public:
		// ��ȡsocket���ص�ַ xxx.xxx.xxx.xxx:xxx
		static char* getSockLocalAddrInfo(int sockfd);

		// ��ȡsocketd�Զ˵�ַ��Ϣ  xxx.xxx.xxx.xxx:xxx
		static char* getSocketPeerAddrInfo(int sockfd);

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_UTILS_H__
