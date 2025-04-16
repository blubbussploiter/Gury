#pragma once

#include <string>
#include "rttr/registration.h"

#include "luabridge.h"

namespace RBX
{
	class SignalInstance
	{
	public:
		
		static G3D::Table<int, lua_State*> connectedLuaSlots;
		
		static void disconnectLuaFunction(lua_State* L, int function);
		static void callLuaFunction(lua_State* L, int function, int narg);
		static void pushLuaFunction(lua_State* L);

		static int connect(lua_State* L);

	public:

		std::string name;

		SignalInstance()
		{
			name = "Unknown";
		}

		RTTR_ENABLE();
	};
}