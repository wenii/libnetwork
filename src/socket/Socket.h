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
		// socket ����
		bool isValide();

		// ��ȡsocket������
		int getSockFD();

	public:
		// ��������
		bool connect(const char* host, const char* serv);

		// ����
		bool listen(const char* host, const char* serv);	

		// ��������
		Socket accept();

		// ��������
		bool recv(char* buf, int* size, int flag = 0);

		// ��������
		bool send(const char* buf, int* size, int flag = 0);

		// �ر�����
		bool close();


	private:
		int _sockfd;		// socket ������  
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
