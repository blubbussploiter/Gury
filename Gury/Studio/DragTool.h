#pragma once

#include "StudioTool.h"
#include "../Gury/Game/Objects/ISelectable.h"

#include "../Gury/Game/Objects/model.h"

namespace RBX
{
	namespace Studio
	{
		class DragTool : public StudioTool
		{
		private:
			bool isDragging;
			bool justReleased;
		public:

			void onDrag();

			void doGraphics(RenderDevice* rd) {}
			void doLogic(UserInput* userInput);

		};
	}
}