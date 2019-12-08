#include <iostream>
#include "../src/socket/Socket.h"
#include <thread>
#include <chrono>
#include <string.h>
int main()
{
	libnetwork::Socket socket;
	socket.setNonblock();
	socket.setTcpNodelay();
	socket.listen("localhost", "9999");
	libnetwork::Socket client = socket.accept();

	char buf[1024] = { 0 };
	int size = 1024;

	while (true)
	{
		int ret = client.recv(buf, &size);
		std::cout << "recv msg:" << buf << "ret:" << ret << std::endl;
		memset(buf, 0, size);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "recv msg:" << buf << std::endl;

	std::cout << "hello libnetwork!" << std::endl;
	return 0;
}