
#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Services/service.h"

#include "instance.h"

namespace RBX
{

	enum ControllerTypes
	{
		None,
		KeyboardLeft,
		KeyboardRight
	};

	class Controller : 
		public RBX::Instance
	{
	private:
		bool isMoving;
		bool isDisabled;
		bool justReleased;
		float speed;
	public:

		bool jumping;

		/* horizontal - W & S, vertical - A & D*/
		float horizontal, vertical;

		float getSpeed() { return speed; }
		void setSpeed(float s) { speed = s; }

		bool moving() { return isMoving; }

		void setmoving(bool m) { isMoving = m; }

		void disable(bool d) { isDisabled = d; }
		bool disabled() { return isDisabled; }

		virtual void handleInput(G3D::UserInput* ui);
		virtual void doMove() {};

		virtual void moveUpdate() {}

		Controller() : speed(0.2f)
		{
			isMoving = 0;
			isDisabled = 0;
		}
		RTTR_ENABLE(RBX::Instance)
	};

	class ControllerService :
		public RBX::Instance,
		public RBX::Service<ControllerService>
	{
	private:
		std::vector<Controller*> controllers;
	public:

		void mapControllers(G3D::UserInput*& ui);

		void updateControllers(G3D::UserInput* ui);
		void addController(Controller* c);

		static ControllerService* get();
		ControllerService()
		{
			setClassName("ControllerService");
			setName("ControllerService");
		}
	};

}