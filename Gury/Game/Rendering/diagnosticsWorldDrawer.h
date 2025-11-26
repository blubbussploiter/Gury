#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Gui/GuiRoot.h"

namespace RBX
{
	namespace Diagnostics
	{
		class Renderer
		{
		private:

			Gui::GuiLabel diag_label0;

		public:

			bool diagnostics_enabled;

			bool kernel_shouldRenderObjects;
			bool joints_shouldRenderOutlines;
			bool joints_shouldRenderWebs;

			void render(RenderDevice* rd);
			void preRender(RenderDevice* rd);
			void render2D(RenderDevice* rd);

			static void diag_jointsWeb(Gui::GuiButton* button);
			static void diag_jointsOutline(Gui::GuiButton* button);

			Renderer()
			{
				diag_label0.position = Vector2(80, 10);
				diag_label0.alignRight = true;
				diag_label0.title = "Joint diagnostics :)";
				diag_label0.textColor = Color3::WHITE;

				/* settings */

				kernel_shouldRenderObjects = 1;

				joints_shouldRenderOutlines = 0;
				joints_shouldRenderWebs = 0;
				
			}
		};

		extern Renderer* get_Renderer();
	}
}