#pragma once

#include <vector>

#include "../Gury/Game/Services/scriptcontext.h"

namespace RBX
{
	namespace Lua
	{

		class YieldingThreads
		{
			class WaitingThread
			{
			public:
				lua_State* thread;

				double requestedDelay;
				double elapsedTime;

				WaitingThread(lua_State* thread, double time) : thread(thread), requestedDelay(time) {}
			};
		private:
			std::vector<WaitingThread*> waitingThreads;
			RBX::ScriptContext* context;
		public:

			void queueWaiter(lua_State* L, double time)
			{
				waitingThreads.push_back(new WaitingThread(L, time));
			}

			void resume(double step) /* called per simulation? */
			{
				for (WaitingThread* t : waitingThreads)
				{
					if (t->thread)
					{
						t->elapsedTime += step;
						if (t->requestedDelay <= t->elapsedTime)
						{
							int oldTop = lua_gettop(t->thread);

							lua_pushnumber(t->thread, t->elapsedTime);
							context->resumeProtected(t->thread, 0);
							lua_settop(t->thread, oldTop);

							if (std::find(waitingThreads.begin(), waitingThreads.end(), t) != waitingThreads.end())
							{
								waitingThreads.erase(std::remove(waitingThreads.begin(), waitingThreads.end(), t));
							}

						}
					}
				}
			}

			bool isYielding(lua_State* L)
			{
				for (WaitingThread* t : waitingThreads)
				{
					if (t->thread == L)
					{
						return 1;
					}
				}
				return 0;
			}

			YieldingThreads(RBX::ScriptContext* context) : context(context) {}
		};
	}
}