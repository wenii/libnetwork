#include "Socket.h"
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "Log.h"
using namespace libnetwork;

Socket::Socket()
	: _sockfd(0)
{

}

Socket::~Socket()
{

}

bool Socket::connect(const char* host, const char* port)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = nullptr;
	int n = getaddrinfo(host, port, &hints, &res);
	if (n != 0)
	{
		Log::error("getaddrinfo() failed. host:%s, port:%s error:%s", host, port, gai_strerror(n));
		return false;
	}
	int sockfd = 0;
	struct addrinfo* head = res;
	do {
		sockfd = socket(head->ai_family, head->ai_socktype, head->ai_protocol);
		if (sockfd < 0)
		{
			continue;
		}
		if (::connect(sockfd, head->ai_addr, head->ai_addrlen) == 0)
		{
			break;
		}
		::close(sockfd);
		
		head = res->ai_next;
	} while (head != nullptr);

	freeaddrinfo(res);

	if (head == nullptr)
	{
		Log::error("connect error for :%s, %s", host, port);
		return false;
	}

	_sockfd = sockfd;
	return true;
}