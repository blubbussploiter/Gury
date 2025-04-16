#include "luabridge.h"

template<typename Class>
void RBX::Lua::SharedPtrBridge<Class>::pushObject(lua_State* L, Class* object)
{
	if (object) {
		lua_pushlightuserdata(L, (void*)object);
		luaL_getmetatable(L, className);
		lua_setmetatable(L, -2);
	}
	else
	{
		lua_pushnil(L);
	}
}

template<typename Class>
Class* RBX::Lua::SharedPtrBridge<Class>::getPtr(lua_State* L, int index)
{
	return (Class*)luaL_checkudata(L, index, className);
}

template<typename Class>
void RBX::Lua::SharedPtrBridge<Class>::open(lua_State* L)
{
	luaL_newmetatable(L, className);

	lua_pushcfunction(L, on_index);
	lua_setfield(L, -2, "__index");

	RBX::StandardOut::print(RBX::MESSAGE_INFO, "farting type = %s", luaL_typename(L, -1));

	lua_pushcfunction(L, on_newindex);
	lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, on_tostring);
	lua_setfield(L, -2, "__tostring");

	lua_pop(L, 1);
}