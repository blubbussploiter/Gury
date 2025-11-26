
#pragma once

#include "../Gury/Game/Objects/keyboard.h"

namespace RBX
{
	class RightKeyboardController : public RBX::KeyboardController
	{
	public:
		void handleInput(UserInput* ui);
	};
}