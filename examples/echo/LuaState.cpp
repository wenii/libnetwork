#include "LuaState.h"
#include <stdio.h>

#include "lua.hpp"
LuaState::LuaState()
{
	_l = luaL_newstate();
}

LuaState::~LuaState()
{
	lua_close(_l);
}

int call_lua_add(lua_State* L)
{
	//lua_getglobal(L, "add"); //��ȡadd����
	//lua_pushnumber(L, 123); //��һ����������ջ
	//lua_pushnumber(L, 456); //�ڶ�����������ջ
	//lua_pcall(L, 2, 1, 0); //���ú�����2��������1������ֵ
	//int sum = (int)lua_tonumber(L, -1); //��ȡջ��Ԫ�أ������
	//lua_pop(L, 1); //ջ��Ԫ�س�ջ
	//return sum;

	lua_getglobal(L, "test");
	lua_pcall(L, 0, 0, 0);
}

void LuaState::test()
{
	luaL_openlibs(_l); //����lua���к�����
	luaL_dofile(_l, "test2.lua"); //ִ��"Test.lua"�ļ��еĴ���
	printf("%d\n", call_lua_add(_l));

	/*luaL_openlibs(_l);
	int ret = luaL_loadfile(_l, "/home/dev/projects/libnetwork/c96395e3-ea2a-4f36-8ab9-b7e2706016cc/out/build/Linux-Debug/examples/echo/test2.lua");
	ret = lua_getglobal(_l, "test");
	if (!lua_isfunction(_l, -1))
	{
		printf("%s\n", lua_tostring(_l, -1));
	}
	if (lua_pcall(_l, 0, 0, 0))
	{
		printf("error %s\n", lua_tostring(_l, -1));
	}*/

	/*ret = lua_getfield(_l, -1, "f");
	if (!lua_isfunction(_l, -1))
	{
		printf("function is not exist.\n");
	}
	if (lua_pcall(_l, 0, 0, 0))
	{
		printf("error %s\n", lua_tostring(_l, -1));
	}*/

}