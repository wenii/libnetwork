#include "Socket.h"
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "Log.h"
#include "SocketUtils.h"

using namespace libnetwork;


static const int LISTEN_QUEUE_LEN = 100;

Socket::Socket()
	: _sockfd(0)
{

}

Socket::Socket(int sockfd)
	: _sockfd(sockfd)
{

}

Socket::~Socket()
{
}
 
bool Socket::connect(const char* host, const char* serv)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = nullptr;
	const int n = ::getaddrinfo(host, serv, &hints, &res);
	if (n != 0)
	{
		Log::error("getaddrinfo() failed when connect. host:%s, serv:%s error:%s", host, serv, gai_strerror(n));
		return false;
	}
	int sockfd = 0;
	struct addrinfo* head = res;
	do {
		sockfd = ::socket(head->ai_family, head->ai_socktype, head->ai_protocol);
		if (sockfd < 0)
		{
			continue;
		}
		if (::connect(sockfd, head->ai_addr, head->ai_addrlen) == 0)
		{
			Log::info("connect host:%s:%s success.", host, serv);
			break;
		}
		::close(sockfd);
		
		head = res->ai_next;
	} while (head != nullptr);

	::freeaddrinfo(res);

	if (head == nullptr)
	{
		Log::error("connect error for :%s, %s", host, serv);
		return false;
	}

	_sockfd = sockfd;
	return true;
}

bool Socket::listen(const char* host, const char* serv)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = nullptr;
	const int n = ::getaddrinfo(host, serv, &hints, &res);
	if (n != 0)
	{
		Log::error("getaddrinfo() failed when listen. host:%s, serv:%s error:%s", host, serv, gai_strerror(n));
		return false;
	}
	int listenfd = 0;
	struct addrinfo* head = res;
	do {
		listenfd = ::socket(head->ai_family, head->ai_socktype, head->ai_protocol);
		if (listenfd < 0)
		{
			continue;
		}
		
		const int on = 1;
		::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		
		if (::bind(listenfd, head->ai_addr, head->ai_addrlen) == 0)
		{
			Log::info("bind host:%s:%s success.", host, serv);
			break;
		}

		::close(listenfd);

		head = res->ai_next;
	} while (head != nullptr);
	
	::freeaddrinfo(res);

	if (head == nullptr)
	{
		Log::error("bind error for %s:%s", host, serv);
		return false;
	}

	if (::listen(listenfd, LISTEN_QUEUE_LEN) != 0)
	{
		Log::error("listen error for %s:%s", host, serv);
		return false;
	}


	_sockfd = listenfd;

	Log::info("listen success for %s:%s fd:%d, addr:%s", host, serv, listenfd, SocketUtils::getSockLocalAddrInfo(listenfd));

	return true;
}

Socket Socket::accept()
{	
	int connfd = 0;
	if ((connfd = ::accept(_sockfd, nullptr, nullptr)) > 0)
	{
		Log::info("accept success for connfd:%d client:%s", connfd, SocketUtils::getSocketPeerAddrInfo(connfd));
		return Socket(connfd);
	}
	return Socket();
}

bool Socket::isValide()
{
	_sockfd > 0;
}

int Socket::getSockFD()
{
	return _sockfd;
}

bool Socket::recv(char* buf, int* size, int flag)
{
	const int bufSize = *size;
	int recvSize = 0;
	do {
		ssize_t n = ::recv(_sockfd, buf + recvSize, bufSize - recvSize, flag);
		if (n > 0)
		{
			recvSize += n;
		}
		else if (n == -1)
		{
			if (errno == EINTR)
			{
				continue;		// 中断，重试
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;			// 没有数据可读，返回
			}
			else
			{
				Log::error("sockfd:%d recv error.", _sockfd);		// socket 读取错误
				return false;
			}
		}
		else
		{
			Log::info("sockfd:%d recv error, socket closed by peer.", _sockfd);	// socket 被关闭
			return false;
		}

	} while (recvSize < bufSize);
	
	*size = recvSize;

	return true;
}

bool Socket::send(const char* buf, int* size, int flag)
{
	const int bufSize = *size;
	int sendSise = 0;
	do {
		ssize_t n = ::send(_sockfd, buf + sendSise, bufSize - sendSise, flag);
		if (n >= 0)
		{
			sendSise += n;
		}
		else
		{
			if (errno == EINTR)
			{
				continue;		// 中断，重试
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;			// socket缓冲区没有空间可以写，返回
			}
			else
			{
				Log::error("sockfd:%d send error.", _sockfd);		// socket 发送错误
				return false;
			}
		}
	} while (sendSise < bufSize);

	*size = sendSise;

	return true;
}
  
bool Socket::close()
{
	if (::shutdown(_sockfd, SHUT_WR) != 0)
	{
		Log::error("close socket error.sockfd:%d", _sockfd);
		return false;
	}
	return true;
}


