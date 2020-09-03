#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
namespace libnetwork
{
	class Socket
	{
	public:
		// ��������  
		static int connect(const char* host, const char* port);

		// ����
		static int listen(const char* addr, const char* port, int proto, int backlog);
		static int listenToPortWithIPv4(const char* addr, const char* port, int backlog);
		static int listenToPortWithIPv6(const char* addr, const char* port, int backlog);

		// ��������
		static int accept(int fd, char* addr, int size);

		// ��������
		static bool recv(int fd, char* buf, int* size, int flag = 0);

		// ��������
		static bool send(int fd, const char* buf, int* size, int flag = 0);

		// �ر�����
		static void close(int fd);

		// �ر�д
		static void shutdown(int fd);

	private:
		// ��ȡsocketѡ��
		static bool getSockOpt(int fd, int level, int optname, void* optval, int* optlen);

		// ����socketѡ��
		static bool setSockOpt(int fd, int level, int optname, const void* optval, int optlen);

		// ���ò�����IPV6����IPV4
		static bool setIPV6Only(int fd);

		// ���õ�ַ������
		static bool setReuseAddr(int fd);

		// ���ý���Nagle�㷨
		static bool setNodelay(int fd);

		// �����׽��ַ�����
		static bool setNonBlock(int fd);
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
