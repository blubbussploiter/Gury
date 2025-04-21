#pragma once

#include <tuple>

#include "../Gury/Game/Services/scriptcontext.h"

namespace RBX
{
	template<class F, class...Ts, std::size_t...Is>
	void for_each_in_tuple(const std::tuple<Ts...>& tuple, F func, std::index_sequence<Is...>) {
		using expander = int[];
		(void)expander {
			0, ((void)func(std::get<Is>(tuple)), 0)...
		};
	}

	template<class F, class...Ts>
	void for_each_in_tuple(const std::tuple<Ts...>& tuple, F func) {
		for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
	}

	template <typename... Args>
	static void pushArgs(lua_State* L, const std::tuple<Args...> arguments, size_t nargs)
	{
		int cArg = 0;
		for_each_in_tuple(arguments, [&](const auto& arg)
			{
				if (cArg != 0)
				{
					if (typeid(arg) == typeid(std::string))
					{
						std::string string = *(std::string*)(&arg);
						lua_pushstring(L, string.c_str());
					}
					else if (typeid(arg) == typeid(int))
					{
						lua_pushinteger(L, *(int*)(&arg));
					}
					else if (typeid(arg) == typeid(float))
					{
						lua_pushnumber(L, *(float*)(&arg));
					}
					else if (typeid(arg) == typeid(double))
					{
						lua_pushnumber(L, *(double*)(&arg));
					}
					else if (typeid(arg) == typeid(bool))
					{
						lua_pushboolean(L, *(bool*)(&arg));
					}
					else if (typeid(arg) == typeid(Instance*))
					{
						Instance* instance = *(Instance**)(&arg);
						RBX::Lua::SharedPtrBridge<Instance>::pushObject(L, instance);
					}
					else if (typeid(arg) == typeid(const char*))
					{
						lua_pushstring(L, *(const char**)(&arg));
					}
					else
					{
						lua_pushnil(L);
					}

				}
				cArg++;
			});
	}

	namespace Reflection
	{
		class FunctionScriptSlot
		{
		private:
			lua_State* L;
			int functionIndex;
			ScriptContext* context;
		public:

			template <typename... Args>
			void operator()(Args... args)
			{
				std::tuple<Args...> arguments(args...);
				size_t nargs = sizeof...(args);

				lua_State* eventThread = lua_newthread(L);
				if (eventThread)
				{
					lua_rawgeti(L, LUA_REGISTRYINDEX, functionIndex);
					lua_xmove(L, eventThread, 1);
					pushArgs(eventThread, arguments, nargs);

					int result = context->resumeProtected(eventThread, nargs-1); /* -1 for this */
					if (result == LUA_ERRRUN)
					{
						SignalInstance::disconnectLuaFunction(eventThread, functionIndex);
					}
					lua_settop(eventThread, 0);
				}
				else
				{
					throw std::runtime_error("Unable to create a new event handler thread");
				}
			}

			FunctionScriptSlot(lua_State* L, int functionIndex)
			{
				this->functionIndex = functionIndex;
				this->L = L;
				context = ScriptContext::get();
			}
		};
	}
}