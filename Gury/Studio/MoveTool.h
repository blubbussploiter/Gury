#pragma once

#include "StudioTool.h"

#include "../Gury/Game/Objects/ISelectable.h"
#include "../Gury/Game/Objects/instance.h"

namespace RBX
{
	namespace Studio
	{
		class MoveTool : public StudioTool
		{
		private:
			bool isMoving;
			bool justReleased;
		public:

			void onMove();

			void doGraphics(RenderDevice* rd);
			void doLogic(UserInput* userInput);

		};
	}
}