
#pragma once

#include <string>

extern "C"
{
#include "lapi.h"
#include "ldo.h"
#include "lauxlib.h"
#include "lstate.h"
#include "lualib.h"
}

#define RBX_LUA_REGISTER(L, c) RBX::Lua::Bridge<c>::open(L)
#define RBX_PTR_LUA_REGISTER(L, c) RBX::Lua::SharedPtrBridge<c>::open(L)

#define RBX_LUA_REGISTER_NAME(c, n) const char* RBX::Lua::Bridge<c>::className = n; \
  const char* RBX::Lua::SharedPtrBridge<c>::className = n;

namespace RBX
{
	namespace Lua
	{

		/* uses normal userdata, non pointers, raw data, for Class styled instances */

		template <typename Class>
		class Bridge
		{
		public:

			static const char* className;

			/* pushers, pullers */

			static Class* pushNewObject(lua_State* L, Class object)
			{
				Class* newObject;
				newObject = (Class*)lua_newuserdata(L, sizeof(Class));
				if (newObject)
					*newObject = object;
				luaL_getmetatable(L, className);
				lua_setmetatable(L, -2);
				return newObject;
			}

			static Class* getValue(lua_State* L, int index)
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

			static Class* getObject(lua_State* L, int index)
			{
				return (Class*)luaL_checkudata(L, index, className);
			}

			/* `reflected` roblox methods */

			static int on_index(Class* object, const char* name, lua_State* L);
			static int on_newindex(Class* object, const char* name, lua_State* L);
			static int on_tostring(Class* object, lua_State* L);

			/* base lua __methods */

			static int on_index(lua_State* L)
			{
				Class* object;
				const char* name;

				object = getObject(L, 1);
				name = luaL_checkstring(L, 2);

				return on_index(object, name, L);
			}

			static int on_newindex(lua_State* L)
			{
				Class* object;
				const char* name;

				object = getObject(L, 1);
				name = luaL_checkstring(L, 2);

				return on_newindex(object, name, L);
			}

			static int on_tostring(lua_State* L)
			{
				Class* object;
				object = getObject(L, 1);

				return on_tostring(object, L);
			}

			/* math __methods */

			static int on_add(lua_State* L);
			static int on_sub(lua_State* L);
			static int on_div(lua_State* L);
			static int on_mul(lua_State* L);
			static int on_unm(lua_State* L);

			static void openMath(lua_State* L)
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

			static void open(lua_State* L)
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

		};

		/* using `shared ptrs`, instead of normal lua userdatas, use lightuserdata (for pointers!) */

		template <typename Class>
		class SharedPtrBridge
		{
		public:

			static const char* className;

			/* pushers and pullers */

			static void pushObject(lua_State* L, Class* object)
			{
				if (object) {
					lua_pushlightuserdata(L, (void*)object);
					luaL_getmetatable(L, className);
					lua_setmetatable(L, -2);
					RBX::StandardOut::print(RBX::MESSAGE_INFO, "ppush top = %s, %s", luaL_typename(L, -1), className);
				}
				else
				{
					lua_pushnil(L);
				}
			}

			static Class* getPtr(lua_State* L, int index)
			{
				RBX::StandardOut::print(RBX::MESSAGE_INFO, "get type %s at %d", className, index);
				return (Class*)luaL_checkudata(L, index, className);
			}

			/* `reflected` roblox methods */

			static int on_index(Class* object, const char* name, lua_State* L);
			static int on_newindex(Class* object, const char* name, lua_State* L);
			static int on_tostring(Class* object, lua_State* L);

			/* base lua __methods */

			static int on_indexDefinition(lua_State* L)
			{
				Class* object;
				const char* name;

				object = getPtr(L, 1);
				name = luaL_checkstring(L, 2);

				return on_index(object, name, L);
			}

			static int on_newindexDefinition(lua_State* L)
			{
				Class* object;
				const char* name;

				object = getPtr(L, 1);
				name = luaL_checkstring(L, 2);

				return on_newindex(object, name, L);
			}

			static int on_tostringDefinition(lua_State* L)
			{
				Class* object;
				object = getPtr(L, 1);

				return on_tostring(object, L);
			}

			/* registration */

			static void open(lua_State* L)
			{
				luaL_newmetatable(L, className);
				RBX::StandardOut::print(RBX::MESSAGE_INFO, "open for %s", className);

				lua_pushcfunction(L, on_indexDefinition);
				lua_setfield(L, -2, "__index");

				RBX::StandardOut::print(RBX::MESSAGE_INFO, "farting type = %s", luaL_typename(L, -1));

				lua_pushcfunction(L, on_newindexDefinition);
				lua_setfield(L, -2, "__newindex");

				RBX::StandardOut::print(RBX::MESSAGE_INFO, "farting type 1 = %s", luaL_typename(L, -1));

				lua_pushcfunction(L, on_tostringDefinition);
				lua_setfield(L, -2, "__tostring");

				RBX::StandardOut::print(RBX::MESSAGE_INFO, "farting type 2 = %s", luaL_typename(L, -1));

				lua_pop(L, 1);
			}

		};

	}
	/*
	namespace Reflection
	{
		template <typename Class>
		class LuaMetatable
		{
		public:

			static std::string className;

			static int on_index(lua_State* L);
			static int on_newindex(lua_State* L);
			static int on_tostring(lua_State* L);

			static Class* getObject(lua_State* L, int idx)
			{
				return (Class*)luaL_checkudata(L, idx, className.c_str());
			}

			static int pushConObject(lua_State* L, Class* c)
			{
				lua_pushlightuserdata(L, c);

				luaL_getmetatable(L, className.c_str());
				lua_setmetatable(L, -2);

				return 1;
			}

			static int pushObject(lua_State* L, Class* c)
			{
				luaL_getmetatable(L, className.c_str());

				if (lua_isnil(L, -1))
					return 0;

				return pushConObject(L, c);
			}

			static void registerClass(lua_State* L)
			{
				luaL_newmetatable(L, className.c_str());

				lua_pushcfunction(L, on_index);
				lua_setfield(L, -2, "__index");

				lua_pushcfunction(L, on_newindex);
				lua_setfield(L, -2, "__newindex");

				lua_pushcfunction(L, on_tostring);
				lua_setfield(L, -2, "__tostring");

				lua_pop(L, 1);
			}

		};


	}
	*/
}
