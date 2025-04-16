
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/scriptcontext.h"

#include "signaldescriptor.h"

using namespace RBX;
using namespace G3D;

/* Sloptimus prime */

void instanceStringSignalCallbackWrapper(Instance* instance, std::string string)
{
	Table<int, lua_State*> slots = RBX::SignalDesc<void(Instance*, std::string)>::connectedLuaSlots;
	Array<int> keys = slots.getKeys();

	for (int i = 0; i < keys.size(); i++) {

		int functionIndex = keys[i];
		lua_State* thread = slots[functionIndex];

		if (thread) {

			lua_rawgeti(thread, LUA_REGISTRYINDEX, functionIndex);
			lua_pushstring(thread, string.c_str());

			RBX::SignalDesc<void(Instance*, std::string)>::callLuaFunction(thread, functionIndex, 1);
		}
	}
}

void SignalDesc<void(Instance*, std::string)>::connectGeneric(lua_State* L)
{
	if (!_connectionEstablished) {
		ourSignal->connect(instanceStringSignalCallbackWrapper);
		_connectionEstablished = true;
	}
	pushLuaFunction(L);
}