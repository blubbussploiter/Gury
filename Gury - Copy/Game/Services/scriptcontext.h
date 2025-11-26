#pragma once

#include <vector>

#include "../Gury/Game/Lua/instancebridge.h"
#include "../Gury/Game/Lua/luaAllocator.h"
#include "../Gury/Game/Objects/basescript.h"

#include "service.h"

namespace RBX
{
	class ScriptContext : public RBX::Instance
	{
	private:
		std::vector<BaseScript*> scripts;

		lua_State* globalState;
		LuaAllocator* lua_Allocator;

		bool scriptsDisabled;
	public:

		lua_State* getGlobalState()
		{
			return globalState;
		}

		void openState();

		void executeInNewThread(std::string script);
		void execute(std::string script); /* try: except wrapper for executeInNewThread */

		int getNumScripts()
		{
			return scripts.size();
		}

		int resume(lua_State* L, int narg);
		/* resume nested within a try - catch */
		int resumeProtected(lua_State* L, int narg);

		static void onRunServiceReset();

		void splashAllocatorStats();

		void onWorkspaceDescendentAdded(RBX::Instance* descendent);
		void onWorkspaceDescendentRemoved(RBX::Instance* descendent);

		void runScripts();
		void runScript(RBX::BaseScript* script);

		void resetScriptThreads();

		void closeState();

		void doError(std::string error);

		static ScriptContext* get();

		/* custom funcs */

		static int wait(lua_State* L);
		static int print(lua_State* L);

		static int instanceNew(lua_State* L);

		ScriptContext()
		{
			setName("Script Context");
			setClassName("ScriptContext");
		}
		RTTR_ENABLE(RBX::Instance);
	};
}