#include <iostream>
#include "../src/Socket.h"
#include <thread>
#include <chrono>
#include <string.h>
#include "../src/TcpServer.h"
#include "../src/EventLoop.h"
#include "TestServer.h"


int main()
{
	libnetwork::TestServer testServer;
	testServer.start();
	return 0;
}