#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
namespace libnetwork
{
	class Socket
	{
	public:
		// ���캯��
		Socket();

		// ��������
		~Socket();

	public:
		// ��������
		bool connect(const char* host, const char* serv);

		// ����
		bool listen(const char* host, const char* serv);	

	private:
		int _sockfd;		// socket ������  
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
