
#pragma once

#include "../Gury/Game/rbx.h"

namespace RBX
{
	class CrashReporter
	{
	public:
		static CrashReporter* get();
		int processException(_EXCEPTION_POINTERS* ExceptionInfo);
		void start();
	};
}