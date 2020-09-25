#include "TestServer.h"
#include <stdio.h>
#include <string.h>
#include "TestTimer.h"
using namespace libnetwork;

static bool isAddTimer = false;

TestServer::TestServer()
{

}

int TestServer::onRecv(long long clientID, const char* buf, int size)
{
	//printf("onRecv.clientID:%lld size:%d\n", clientID, size);
	char buffer[1024] = { 0 };
	int copySize = size < 1024 ? size : 1024;
	memcpy(buffer, buf, copySize);
	//printf("recv msg:%s\n", buffer);


	if (!strcmp(buffer, "close"))
	{
		printf("disconnect client:%lld\n", clientID);
		disconnect(clientID);
	}
	else
	{
		send(clientID, "abcd", 4);
	}
	return 0;
}

void TestServer::onAccept(long long clientID)
{
	printf("onAccept.clientID:%lld\n", clientID);
}

void TestServer::onDisconnect(long long clientID)
{
	printf("onDisconnect.clientID:%lld\n", clientID);
}

void TestServer::update(int dt)
{
	if (!isAddTimer)
	{
		//addTimer(new TestTimer(0, 1));
		addTimer(new TestTimer(1));

		isAddTimer = true;
	}
}