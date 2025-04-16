
#include "../Services/stdout.h"

#include "signaldescriptor.h"

int RBX::Lua::SharedPtrBridge<RBX::SignalInstance>::on_tostring(SignalInstance* object, lua_State* L)
{
	RBX::StandardOut::print(RBX::MESSAGE_WARNING, "Tostring Signal ");
	lua_pushfstring(L, "Signal Instance %s", object->name.c_str());
	return 1;
}

int RBX::Lua::SharedPtrBridge<RBX::SignalInstance>::on_index(SignalInstance* object, const char* name, lua_State* L)
{
	if (!strcmp(name, "connect")) {
		lua_pushcfunction(L, &object->connect);
		return 1;
	}
	return 0;
}

int RBX::Lua::SharedPtrBridge<RBX::SignalInstance>::on_newindex(SignalInstance* object, const char* name, lua_State* L)
{
	return 0;
}