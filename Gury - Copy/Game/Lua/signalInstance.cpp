
#include "../Objects/instance.h"
#include "../Gury/Game/Services/scriptcontext.h"

#include "signalInstance.h"
#include "signaldescriptor.h"

using namespace RBX;
using namespace G3D;

Table<int, lua_State*> RBX::SignalInstance::connectedLuaSlots = Table<int, lua_State*>();

void RBX::SignalInstance::disconnectLuaFunction(lua_State* L, int function)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, function);

	if (!lua_isnil(L, -1)) {
		lua_pop(L, 1);
		luaL_unref(L, LUA_REGISTRYINDEX, function);
	}
}

int RBX::SignalInstance::pushLuaFunction(lua_State* L) {

	lua_pushvalue(L, 2);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

int RBX::SignalInstance::connect(lua_State* L)
{
	SignalInstance* signal = Lua::SharedPtrBridge<SignalInstance>::getPtr(L, 1);

	if (signal)
	{

	}

	return 0;
}
