
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

void RBX::SignalInstance::callLuaFunction(lua_State* L, int function, int narg) {

	int result = lua_pcall(L, narg, -1, 0);

	if (result) {

		std::string error = lua_tostring(L, -1);
		ScriptContext::get()->doError(error);
		disconnectLuaFunction(L, function);
		lua_pop(L, 1);
	}
}

void RBX::SignalInstance::pushLuaFunction(lua_State* L) {

	lua_pushvalue(L, 2);
	int i = luaL_ref(L, LUA_REGISTRYINDEX);

	connectedLuaSlots.set(i, L); /* Push index of this */
}

int RBX::SignalInstance::connect(lua_State* L)
{
	SignalInstance* signal = Lua::SharedPtrBridge<SignalInstance>::getPtr(L, 1);
	rttr::type base = rttr::detail::get_type_from_instance(signal);

	/* Holy slop */

	if (IsAAny<SignalInstance, SignalDesc<void(RBX::Instance*, std::string)>>(signal)) {
		toAny<SignalInstance, SignalDesc<void(RBX::Instance*, std::string)>>(signal)->connectGeneric(L);
	}

	if (IsAAny<SignalInstance, SignalDesc<void(RBX::Instance*)>>(signal)) {
		// toAny<SignalInstance, SignalDesc<void(RBX::Instance*)>>(signal)->connectGeneric(L);
	}

	return 0;
}
