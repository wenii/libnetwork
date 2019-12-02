#include <iostream>
#include "../src/socket/Socket.h"
int main()
{
	libnetwork::Socket socket;
	//socket.connect("www.baidu.com", "80");
	socket.listen("localhost", "9999");
	std::cout << "hello libnetwork!" << std::endl;
	return 0;
}