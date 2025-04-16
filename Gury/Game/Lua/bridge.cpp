
#include "luabridge.h"

template<typename Class>
void RBX::Lua::Bridge<Class>::open(lua_State* L)
{
	luaL_newmetatable(L, className);

	lua_pushcfunction(L, on_index);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, on_newindex);
	lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, on_tostring);
	lua_setfield(L, -2, "__tostring");

	openMath(L);

	lua_pop(L, 1);
}

template<typename Class>
void RBX::Lua::Bridge<Class>::openMath(lua_State* L)
{
	lua_pushcfunction(L, on_add);
	lua_setfield(L, -2, "__add");

	lua_pushcfunction(L, on_sub);
	lua_setfield(L, -2, "__sub");

	lua_pushcfunction(L, on_mul);
	lua_setfield(L, -2, "__mul");

	lua_pushcfunction(L, on_div);
	lua_setfield(L, -2, "__div");

	lua_pushcfunction(L, on_unm);
	lua_setfield(L, -2, "__unm");
}

template<typename Class>
Class* RBX::Lua::Bridge<Class>::getObject(lua_State* L, int index)
{
	return (Class*)luaL_checkudata(L, index, className);
}

template<typename Class>
Class* RBX::Lua::Bridge<Class>::getValue(lua_State* L, int index)
{
	const void* v3;
	Class* v4;

	v3 = lua_touserdata(L, index);
	if (v3)
	{
		if (lua_getmetatable(L, index))
		{
			lua_getfield(L, -10000, className);
			if (lua_rawequal(L, -1, -2))
			{
				lua_settop(L, -3);
				v4 = (Class*)v3;
				return v4;
			}
		}
		else
		{
			lua_settop(L, -2);
		}
	}
	return 0;
}

template<typename Class>
Class* RBX::Lua::Bridge<Class>::pushNewObject(lua_State* L, Class object)
{
	Class* newObject;
	newObject = (Class*)lua_newuserdata(L, sizeof(Class));
	if (newObject)
		*newObject = object;
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
	return newObject;
}