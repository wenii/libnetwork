#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
namespace libnetwork
{
	class Socket
	{
	public:
		// ���캯��
		Socket();
		Socket(int sockfd);

		// ��������
		~Socket();

	public:
		// ��������
		bool connect(const char* host, const char* serv);

		// ����
		bool listen(const char* host, const char* serv);	

		// ��������
		Socket accept();

	public:
		// socket ����
		bool isValide();

		// ��ȡsocket������
		int getSockFD();


	private:
		int _sockfd;		// socket ������  
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
