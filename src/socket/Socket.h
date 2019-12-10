#ifndef __LIB_NETWORK_SOCKET_H__
#define __LIB_NETWORK_SOCKET_H__
#include <time.h>
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
		bool isValide() const;

		// ��ȡsocket������
		int getSockFD() const;

	public:
		// ���ý���Nagle�㷨
		void setTcpNodelay();
		
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
		// ��ȡsocketѡ��
		bool getSockOpt(int level, int optname, void* optval, int* optlen);

		// ����socketѡ��
		bool setSockOpt(int level, int optname, const void* optval, int optlen);

		// �����û��Զ���socket����
		void setSockOptCustom();

	private:
		int _sockfd;						// socket ������  
		int _nodelay;						// �Ƿ�ʹ��Nagle�㷨	TCP_NODELAY
	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_SOCKET_H__
