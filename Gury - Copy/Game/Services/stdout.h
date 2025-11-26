#pragma once

#include "../Gury/Game/log.h"

namespace RBX
{

	class StandardOut
	{
	public:
		static void print(MessageType type, const char* format, ...);
	};
}