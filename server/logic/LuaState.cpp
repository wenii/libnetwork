#include "LuaState.h"
#include "lua.hpp"
LuaState::LuaState()
{
	_l = luaL_newstate();
	luaL_openlibs(_l); //载入lua所有函数库
}

LuaState::~LuaState()
{
	lua_close(_l);
}

void LuaState::loadFile(const char* fineName)
{
	luaL_dofile(_l, fineName); 
}

void LuaState::onAccept(uint32_t connID)
{
	lua_getglobal(_l, "onAccept");
	lua_pushinteger(_l, connID);
	lua_pcall(_l, 1, 0, 0);
}

void LuaState::onDisconnect(uint32_t connID)
{
	lua_getglobal(_l, "onDisconnect");
	lua_pushinteger(_l, connID);
	lua_pcall(_l, 1, 0, 0);
}

void LuaState::update(int dt)
{
	lua_getglobal(_l, "update");
	lua_pushinteger(_l, dt);
	lua_pcall(_l, 1, 0, 0);
}

void LuaState::onMessage(uint32_t gateID, uint32_t clientID, const char* data, int size)
{
	lua_getglobal(_l, "onRouter");
	lua_pushinteger(_l, gateID);
	lua_pushinteger(_l, clientID);
	lua_pushlstring(_l, data, size);
	lua_pushinteger(_l, size);
	lua_pcall(_l, 4, 0, 0);

}