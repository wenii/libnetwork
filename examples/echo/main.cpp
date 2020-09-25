//#define LUA_PATH_DEFAULT "."
#include "EchoServer.h"
#include "LuaState.h"
int main()
{
	//EchoServer echoServer;
	//echoServer.start();

	LuaState state;
	state.test();
	return 0;
}