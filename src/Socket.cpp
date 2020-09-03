#include "Socket.h"
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "Log.h"

using namespace libnetwork;

int Socket::connect(const char* host, const char* serv)
{
	int sockfd = -1;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = nullptr;
	const int n = ::getaddrinfo(host, serv, &hints, &res);
	if (n != 0)
	{
		Log::error("getaddrinfo() failed when connect. host:%s, serv:%s error:%s", host, serv, gai_strerror(n));
		return -1;
	}

	struct addrinfo* head = res;
	bool connectSuccess = false;
	for (; head != nullptr; head = head->ai_next)
	{
		if((sockfd = ::socket(head->ai_family, head->ai_socktype, head->ai_protocol)) == -1) continue;

		if (::connect(sockfd, head->ai_addr, head->ai_addrlen) == -1) goto err;

		if (!setNodelay(sockfd)) goto err;
		if (!setNonBlock(sockfd)) goto err;
		
		Log::info("connect host:%s:%s success.", host, serv);
		connectSuccess = true;
		break;
		
	err:
		::close(sockfd);
		sockfd = -1;
	}

	::freeaddrinfo(res);

	if (!connectSuccess)
	{
		Log::error("connect error for :%s, %s", host, serv);
		return -1;
	}

	return sockfd;
}


int Socket::listenToPortWithIPv4(const char* addr, const char* port, int backlog)
{
	return listen(addr, port, AF_INET, backlog);
}

int Socket::listenToPortWithIPv6(const char* addr, const char* port, int backlog)
{
	return listen(addr, port, AF_INET6, backlog);
}

bool Socket::setIPV6Only(int fd)
{
	int on = 1;
	return setSockOpt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
}

bool Socket::setReuseAddr(int fd)
{
	int on = 1;
	return setSockOpt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}


bool Socket::setNodelay(int fd)
{
	int on = 1;
	return setSockOpt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
}

bool Socket::setNonBlock(int fd)
{
	int flags = 0;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) goto err;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1) goto err;
	return true;

err:
	Log::error("setSockOptCustom:fcntl F_GETFL error.");
	return false;
}

static char* sockntop(const struct sockaddr* sa)
{
	char portStr[8] = { 0 };
	static char str[128] = { 0 };

	memset(portStr, 0, 8);
	memset(str, 0, 128);

	switch (sa->sa_family)
	{
	case AF_INET:
	{
		const struct sockaddr_in* sin = (const struct sockaddr_in*)sa;
		if (::inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) != nullptr)
		{
			snprintf(portStr, sizeof(portStr), ":%d", ntohs(sin->sin_port));
			strcat(str, portStr);
			return str;
		}
		else
		{
			Log::error("inet_ntop AF_INET error.");
		}
		break;
	}
	case AF_INET6:
	{
		const struct sockaddr_in6* sin6 = (const struct sockaddr_in6*)sa;
		if (::inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) != nullptr)
		{
			snprintf(portStr, sizeof(portStr), ":%d", ntohs(sin6->sin6_port));
			strcat(str, portStr);
			return str;
		}
		else
		{
			Log::error("inet_ntop AF_INET6 error.");
		}
		break;
	}
	default:
		Log::info("unkown protocal:%d", sa->sa_family);
		break;
	}
	return nullptr;
}

static bool getSockName(int sockfd, struct sockaddr_storage* ss, socklen_t* sslen)
{
	if (::getsockname(sockfd, (struct sockaddr*)ss, sslen) == 0)
	{
		return true;
	}
	else
	{
		Log::error("getSockName error.sockfd:%d", sockfd);
		return false;
	}
}

static char* getSockLocalAddrInfo(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	if (getSockName(sockfd, &ss, &sslen))
	{
		return sockntop((struct sockaddr*) & ss);
	}
	return nullptr;
}

int Socket::listen(const char* addr, const char* port, int proto, int backlog)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = proto;
	hints.ai_socktype = SOCK_STREAM;

	int sockfd = -1;
	struct addrinfo* res = nullptr;
	int ret = 0;
	if ((ret = ::getaddrinfo(addr, port, &hints, &res)) != 0)
	{
		Log::error("getaddrinfo() failed when connect.error:%s", gai_strerror(ret));
		return -1;
	}

	struct addrinfo* head = res;
	bool bindsuccess = false;
	for(; head != nullptr; head = head->ai_next)
	{
		if ((sockfd = ::socket(head->ai_family, head->ai_socktype, head->ai_protocol)) == -1) continue;
		
		if (proto == AF_INET6) if (!setIPV6Only(sockfd)) goto err;
		if (!setReuseAddr(sockfd)) goto err;
		if (!setNodelay(sockfd)) goto err;
		if (!setNonBlock(sockfd)) goto err;

		if (::bind(sockfd, head->ai_addr, head->ai_addrlen) == -1) goto err;

		bindsuccess = true;
		break;
	err:
		::close(sockfd);
	}
	
	::freeaddrinfo(res);

	if (!bindsuccess)
	{
		Log::error("bind error.");
		return -1;
	}

	if (::listen(sockfd, backlog) != 0)
	{
		Log::error("listen error.");
		return -1;
	}

	Log::info("listen success for %s fd: %d, addr: %s", port, sockfd, getSockLocalAddrInfo(sockfd));
	return sockfd;
}

int Socket::accept(int fd, char* addr, int size)
{
	int connfd = -1;
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	if ((connfd = ::accept(fd, (struct sockaddr*)&ss, &sslen)) > 0)
	{
		// 监听套接字的非阻塞文件状态标志不会继承给accept返回的新套接字，需要显式设置。
		Socket::setNonBlock(connfd);
		const char* addrInfo = sockntop((struct sockaddr*) &ss);
		Log::info("accept success for connfd:%d client:%s", connfd, addrInfo);

		if (addr && size)
			snprintf(addr, size, addrInfo);
	}
	else
	{
		Log::error("accept error.");
	}
	return connfd;
}

bool Socket::recv(int fd, char* buf, int* size, int flag)
{
	const int bufSize = *size;
	int recvSize = 0;
	do {
		ssize_t n = ::recv(fd, buf + recvSize, bufSize - recvSize, flag);
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
				Log::error("sockfd:%d recv error.", fd);		// socket 读取错误
				return false;
			}
		}
		else
		{
			Log::info("sockfd:%d, socket closed by peer.", fd);	// socket 被关闭
			return false;
		}

	} while (recvSize < bufSize);
	
	*size = recvSize;

	return true;
}

bool Socket::send(int fd, const char* buf, int* size, int flag)
{
	const int bufSize = *size;
	int sendSise = 0;
	do {
		ssize_t n = ::send(fd, buf + sendSise, bufSize - sendSise, flag);
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
				Log::error("sockfd:%d send error.", fd);		// socket 发送错误
				return false;
			}
		}
	} while (sendSise < bufSize);

	*size = sendSise;

	return true;
}
  
void Socket::close(int fd)
{
	::close(fd);
}

void Socket::shutdown(int fd)
{
	const int ret = ::shutdown(fd, SHUT_WR);
	if (ret != 0)
	{
		Log::error("shutdown SHUT_WR error");
	}
}

bool Socket::getSockOpt(int fd, int level, int optname, void* optval, int* optlen)
{
	if (::getsockopt(fd, level, optname, optval, (socklen_t*)optlen) != 0)
	{
		Log::error("getsockopt error. level:%d optname:%d", level, optname);
		return false;
	}
	return true;
}

bool Socket::setSockOpt(int fd, int level, int optname, const void* optval, int optlen)
{
	if (::setsockopt(fd, level, optname, optval, optlen) != 0)
	{
		Log::error("setsockopt error. level:%d optname:%d", level, optname);
		return false;
	}
	return true;
}
