
#include "../Gury/Game/Lua/yieldingthreads.h"

#include "../Gury/Game/Objects/model.h"

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/World/runservice.h"

#include "../Gury/Game/Lua/luabridge.h"
#include "../Gury/Game/Lua/vector3bridge.h"
#include "../Gury/Game/Lua/cframebridge.h"

#include "../Gury/Game/classes.h"

#include "scriptcontext.h"

RBX_REGISTERPTRCLASS(RBX::Instance, "Instance");
RBX_REGISTERPTRCLASS(RBX::SignalInstance, "SignalInstance");

int RBX::ScriptContext::instanceNew(lua_State* L)
{
	int nargs = lua_gettop(L);
	const char* instance = luaL_checkstring(L, 1);

	if (instance)
	{
		RBX::Instance* newInstance = RBX::fromName(instance);

		if (nargs == 2)
		{
			newInstance->setParent(RBX::Lua::SharedPtrBridge<RBX::Instance>::getPtr(L, 2));
		}

		RBX::Lua::SharedPtrBridge<RBX::Instance>::pushObject(L, newInstance);
		return 1;
	}

}

void RBX::ScriptContext::openState()
{
	RBX::StandardOut::print(RBX::MESSAGE_INFO, "ScriptContext::openState() opening");
	if (!globalState)
	{
		RunService::get()->onReset.connect(ScriptContext::onRunServiceReset);

		lua_Allocator = new LuaAllocator();
		globalState = lua_newstate(LuaAllocator::alloc, lua_Allocator);

		if (!globalState)
		{
			throw std::exception("Failed to create Lua state");
		}

		luaopen_base(globalState);
		luaopen_math(globalState);
		luaopen_string(globalState);

		luaL_register(globalState, "Vector3", RBX::Lua::Vector3Bridge::classLibrary);
		luaL_register(globalState, "CFrame", RBX::Lua::CoordinateFrameBridge::classLibrary);

		RBX_LUA_REGISTER(globalState, G3D::Vector3);
		RBX_LUA_REGISTER(globalState, G3D::CoordinateFrame);
		RBX_PTR_LUA_REGISTER(globalState, RBX::Instance);
		RBX_PTR_LUA_REGISTER(globalState, RBX::SignalInstance);

		RBX::Lua::SharedPtrBridge<RBX::Instance>::pushObject(globalState, Datamodel::get());
		lua_setglobal(globalState, "game");

		RBX::Lua::SharedPtrBridge<RBX::Instance>::pushObject(globalState, Workspace::get());
		lua_setglobal(globalState, "workspace");

		lua_register(globalState, "wait", &ScriptContext::wait);
		lua_register(globalState, "print", &ScriptContext::print);

		lua_newtable(globalState);
		lua_pushcfunction(globalState, instanceNew);
		lua_setfield(globalState, -2, "new");
		lua_setglobal(globalState, "Instance");

	}
}

void RBX::ScriptContext::executeInNewThread(std::string script)
{
	lua_State* thread = lua_newthread(globalState);
	if (!thread)
	{
		throw std::runtime_error("ScriptContext::execute: Unable to create a new thread");
	}

	RBX::StandardOut::print(RBX::MESSAGE_INFO, "ScriptContext::executeInNewThread %.64s", script.c_str());
	if (luaL_loadbuffer(thread, script.c_str(), script.size(), "="))
	{
		const char* error = lua_tostring(thread, -1);
		lua_settop(thread, 0);

		throw std::runtime_error(error);
	}
	else
	{
		resume(thread, 0);
	}
}

void RBX::ScriptContext::execute(std::string script)
{
	try
	{
		executeInNewThread(script);
	}
	catch (std::runtime_error e)
	{
		RBX::StandardOut::print(RBX::MESSAGE_ERROR, e.what());
	}

}

int RBX::ScriptContext::resume(lua_State* L, int narg)
{
	int status;
	status = lua_resume(L, narg);

	if (status == LUA_YIELD)
	{
		return 1;
	}

	if (status)
	{
		const char* string = lua_tostring(L, -1);
		lua_settop(L, 0);

		throw std::runtime_error(string);
	}

	return 0;
}

int RBX::ScriptContext::resumeProtected(lua_State* L, int narg)
{
	int result = 0;
	try 
	{
		result = resume(L, narg);
	}
	catch (std::exception err)
	{
		RBX::StandardOut::print(RBX::MESSAGE_ERROR, err.what());
	}
	return result;
}

void RBX::ScriptContext::onRunServiceReset()
{
	ScriptContext* scriptContext = ScriptContext::get();
	scriptContext->resetScriptThreads();
}

void RBX::ScriptContext::splashAllocatorStats()
{
	uint32_t heapSize, heapCount, maxHeapSize, maxHeapCount;

	if (lua_Allocator)
	{
		lua_Allocator->getHeapStats(&heapSize, &heapCount, &maxHeapSize, &maxHeapCount);
		RBX::StandardOut::print(RBX::MESSAGE_INFO, "Script Heap Stats: Max Size = %u bytes, Max Count = %u blocks", maxHeapSize, maxHeapCount);
	}
}

void RBX::ScriptContext::onWorkspaceDescendentAdded(RBX::Instance* descendent)
{
	RBX::BaseScript* script = toInstance<RBX::BaseScript>(descendent);
	if (script)
	{
		scripts.push_back(script);
	}
}

void RBX::ScriptContext::onWorkspaceDescendentRemoved(RBX::Instance* descendent)
{
	if (std::find(
		scripts.begin(),
		scripts.end(),
		descendent) != scripts.end())
	{
		scripts.erase(std::remove(
			scripts.begin(),
			scripts.end(),
			descendent));
	}
}

void RBX::ScriptContext::runScript(RBX::BaseScript* script)
{
	lua_State* thread;
	ProtectedString source;

	if (std::find(scripts.begin(), scripts.end(), script) != scripts.end())
	{

		thread = script->establishScriptThread(globalState);

		if (!thread)
		{
			throw std::exception("ScriptContext::runScript: Unable to create a new thread");
		}

		RBX::Lua::SharedPtrBridge<RBX::Instance>::pushObject(thread, script);
		lua_setglobal(thread, "script");

		source = script->getSource();
		std::string raw = source.source;

		if (!raw.empty())
		{

			if (luaL_loadbuffer(thread, raw.c_str(), raw.size(), script->getFullName().c_str()))
			{
				doError(lua_tolstring(thread, -1, 0));
				lua_settop(thread, 0);
				return;
			}
			else
			{
				resume(thread, 0);
				lua_pop(thread, 1);
			}

		}

	}
}

void RBX::ScriptContext::resetScriptThreads()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		BaseScript* script = scripts.at(i);
		if (script)
		{
			script->resetScriptThread(globalState);
		}
	}
	splashAllocatorStats();
}

void RBX::ScriptContext::closeState()
{
	scripts.clear();
	if (globalState)
	{
		lua_close(globalState);
	}
}

void RBX::ScriptContext::doError(std::string error)
{
	RBX::StandardOut::print(RBX::MESSAGE_ERROR, error.c_str());
}

void RBX::ScriptContext::runScripts()
{
	try
	{
		for (unsigned int i = 0; i < scripts.size(); i++)
		{
			RBX::BaseScript* script = scripts.at(i);
			if (script)
			{
				runScript(script);
			}
		}
	}
	catch (std::exception err)
	{
		RBX::StandardOut::print(RBX::MESSAGE_ERROR, err.what());
	}
}

int RBX::ScriptContext::print(lua_State* L)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i = 1; i <= n; i++) {
		const char* s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "
				LUA_QL("print"));
		if (i > 1)
			RBX::Log::writeEntry("&nbsp;", 1); /* html formatted \t */
		RBX::Log::writeEntry(s, 1);
		lua_pop(L, 1);  /* pop result */
	}
	RBX::Log::writeEntry("<br>", 1);
	return 0;
}

int RBX::ScriptContext::wait(lua_State* L)
{
	double timeout;

	timeout = lua_tonumber(L, 1); 
	RBX::Datamodel::get()->yieldingThreads->queueWaiter(L, timeout);

	return lua_yield(L, 0);
}

RBX::ScriptContext* RBX::ScriptContext::get()
{
	RBX::RunService* runService = RBX::RunService::get();
	if (runService->scriptContext) return runService->scriptContext;
	return 0;
}