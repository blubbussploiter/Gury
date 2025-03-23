#pragma once

#include "controller.h"

namespace RBX
{

	class KeyboardController : public RBX::Controller
	{
	private:
		Instances* motors;
	public:
		void doMove();

		void moveUpdate();
		void handleInput(G3D::UserInput* ui);

		void moveCharacter(RBX::Instance* character);
		void bindCharacter(RBX::Instance* character);

		void bindMotors(RBX::Instance* character);

		KeyboardController()
		{
			motors = new Instances();
		}
	};
}