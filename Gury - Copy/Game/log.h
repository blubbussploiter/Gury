#pragma once

#include "rbx.h"

namespace RBX
{
	enum MessageType
	{
		MESSAGE_PRINT,
		MESSAGE_INFO,
		MESSAGE_WARNING,
		MESSAGE_ERROR,
	};
	class Log
	{
	public:
		static void close();
		static void writeEntry(const char* message, bool writeToUi=false);
	};
}
