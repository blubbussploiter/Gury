
#include "../Services/stdout.h"

#include "signaldescriptor.h"

RBX_REGISTERPTRCLASS(RBX::SignalInstance, "SignalInstance");
RBX_REGISTERCLASS(RBX::SignalInstance, "SignalInstance");

int RBX::Lua::Bridge<RBX::SignalInstance*>::on_tostring(SignalInstance** ptr, lua_State* L)
{
	SignalInstance* object = *ptr;
	lua_pushfstring(L, "Signal Instance %s", object->name.c_str());
	return 1;
}

int RBX::Lua::Bridge<RBX::SignalInstance*>::on_index(SignalInstance** ptr, const char* name, lua_State* L)
{
	SignalInstance* object = *ptr;
	if (!strcmp(name, "connect")) {
		lua_pushcfunction(L, &object->connect);
		return 1;
	}
	return 0;
}

int RBX::Lua::Bridge<RBX::SignalInstance*>::on_newindex(SignalInstance** ptr, const char* name, lua_State* L)
{
	return 0;
}