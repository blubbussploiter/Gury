#pragma once

#include "../Gury/Game/Lua/luabridge.h"
#include "../Gury/Game/Services/stdout.h"
#include "instance.h"

namespace RBX
{

	class ProtectedString
	{
	public:
		std::string source;
	};

	class BaseScript : 
		public RBX::Instance
	{
	private:
		ProtectedString source;
		lua_State* scriptThread;
	public:

		void setSource(ProtectedString newSource) {
			source = newSource;
		}

		ProtectedString getSource() {
			return source; 
		}

		void setSourceFromString(std::string newSource) {
			source.source = newSource;
		}

		std::string getSourceAsString() {
			return source.source;
		}

		lua_State* getScriptThread()
		{
			return scriptThread;
		}

		lua_State* establishScriptThread(lua_State* globalState)
		{
			if (!scriptThread)
			{
				scriptThread = lua_newthread(globalState);
			}
			return scriptThread;
		}

		void resetScriptThread(lua_State* globalState)
		{
			scriptThread = 0;
			establishScriptThread(globalState);
		}

		BaseScript()
		{
			scriptThread = 0;
			setClassName("Script");
			setName("Script");
		}

		RBX_CLONE_DEF(BaseScript)
		RTTR_ENABLE(RBX::Instance);
	};
}