#pragma once

#include <map>
#include <functional>
#include <G3DAll.h>

namespace RBX
{
	namespace Studio
	{
		enum ToolTypes
		{
			 Mouse,
			 Move,
			 Resize
		};

		class StudioTool
		{
		public:

			bool isUsing;
			 
			virtual void doLogic(UserInput* userInput) {}

			virtual void doGraphics(RenderDevice* renderDevice) {}

		};

		class StudioNavigator {
		private:
			Table<SDLKey, std::function<void()>> actions;
		public:

			void bindAction(SDLKey hotKey, std::function<void()> action);
			void doActions(UserInput* userInput);

			static void removeSelected();

			static StudioNavigator* get();
		};

		extern StudioTool* current_Tool;

		extern void setToolFromType(ToolTypes type);

		extern bool currentToolType(ToolTypes type);
	}
}