#ifndef __LUA_STATE_H__
#define __LUA_STATE_H__
class lua_State;
class LuaState
{
public:
	LuaState();
	~LuaState();

	void test();
private:
	lua_State* _l;
};


#endif // !__LUA_STATE_H__
