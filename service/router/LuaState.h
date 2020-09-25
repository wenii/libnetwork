#ifndef __LUA_STATE_H__
#define __LUA_STATE_H__
#include <stdint.h>
class lua_State;

class LuaState
{
public:
	LuaState();
	~LuaState();
public:
	void loadFile(const char* fineName);
	void onAccept(uint32_t connID);
	void onDisconnect(uint32_t connID);
	void update(int dt);
	void onRouter(uint32_t gateID, uint32_t clientID, const char* data, int size);
public:
	lua_State* _l;
};

#endif // !__LUA_STATE_H__
