#pragma once

#include <vector>

#include "../Gury/Game/Lua/instancebridge.h"
#include "../Gury/Game/Objects/basescript.h"

#include "service.h"

namespace RBX
{
	class ScriptContext : public RBX::Instance
	{
	private:
		std::vector<BaseScript*> scripts;
		lua_State* globalState;
		bool scriptsDisabled;
	public:

		void openState();

		void executeInNewThread(std::string script);
		void execute(std::string script); /* try: except wrapper for executeInNewThread */

		int resume(lua_State* L, int narg);
		/* resume nested within a try - catch */
		int resumeProtected(lua_State* L, int narg);

		void onWorkspaceDescendentAdded(RBX::Instance* descendent);
		void onWorkspaceDescendentRemoved(RBX::Instance* descendent);

		void runScripts();
		void runScript(RBX::BaseScript* script);

		void close();

		static ScriptContext* get();

		/* custom funcs */

		static int wait(lua_State* L);
		static int print(lua_State* L);

		ScriptContext()
		{
			setName("Script Context");
			setClassName("ScriptContext");
		}
		RTTR_ENABLE(RBX::Instance);
	};
}