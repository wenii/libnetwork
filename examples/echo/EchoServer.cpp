#include "EchoServer.h"
#include <iostream>
#include <string>
using namespace libnetwork;


int EchoServer::onRecv(long long clientID, const char* buf, int size)
{
	std::string data(buf, size);
	std::cout << "[" << clientID << "] " << data.c_str() << std::endl;
	send(clientID, buf, size);
	return size;
}

void EchoServer::onAccept(long long clientID)
{
	std::cout << "accept a client. ID:" << clientID << std::endl;
}

void EchoServer::onDisconnect(long long clientID)
{
	std::cout << "client[" << clientID << "] disconnect" << std::endl;
}

void EchoServer::update(int dt)
{

}