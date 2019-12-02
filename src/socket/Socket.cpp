#include "Socket.h"
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "Log.h"
using namespace libnetwork;


static const int LISTEN_QUEUE_LEN = 100;

Socket::Socket()
	: _sockfd(0)
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

	Log::info("listen success for %s:%s fd:%d", host, serv, listenfd);

	return true;
}