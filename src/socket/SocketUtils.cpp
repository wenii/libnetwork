#include "SocketUtils.h"
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "Log.h"
using namespace libnetwork;


static char* sockntop(const struct sockaddr* sa)
{
	char portStr[8] = { 0 };
	static char str[128] = { 0 };

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

static bool getPeerName(int sockfd, struct sockaddr_storage* ss, socklen_t* sslen)
{
	if (::getpeername(sockfd, (struct sockaddr*)ss, sslen) == 0)
	{
		return true;
	}
	else
	{
		Log::error("getPeerName error.sockfd:%d", sockfd);
		return false;
	}
}

char* SocketUtils::getSockLocalAddrInfo(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	if (getSockName(sockfd, &ss, &sslen))
	{
		return sockntop((struct sockaddr*)&ss);
	}
	return nullptr;
}

char* SocketUtils::getSocketPeerAddrInfo(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t sslen = sizeof(struct sockaddr_storage);
	if (getPeerName(sockfd, &ss, &sslen))
	{
		return sockntop((struct sockaddr*)&ss);
	}
	return nullptr;
}
