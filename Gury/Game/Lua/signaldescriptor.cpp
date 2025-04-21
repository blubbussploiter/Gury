
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/scriptcontext.h"
#include "../Gury/Game/Reflection/scriptSlot.h"

#include "signaldescriptor.h"

using namespace RBX;
using namespace G3D;

template<typename signalType>
void doConnect(lua_State* L, boost::signal<signalType>* ourSignal)
{
	Reflection::FunctionScriptSlot slot(L, SignalInstance::pushLuaFunction(L));
	ourSignal->connect(slot);
}

void SignalDesc<void(Instance*, std::string)>::connectGeneric(lua_State* L)
{
	doConnect<void(Instance*, std::string)>(L, ourSignal);
}

void SignalDesc<void(Instance*, Instance*)>::connectGeneric(lua_State* L)
{
	doConnect<void(Instance*, Instance*)>(L, ourSignal);
}