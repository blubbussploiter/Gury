#include "diagnosticsWorldDrawer.h"

#include "../Gury/Kernel/jointsservice.h"

RBX::Diagnostics::Renderer* renderer;

RBX::Diagnostics::Renderer* RBX::Diagnostics::get_Renderer()
{
	if (!renderer)
	{
		renderer = new RBX::Diagnostics::Renderer();
	}
	return renderer;
}

void RBX::Diagnostics::Renderer::render(RenderDevice* rd)
{
	if (diagnostics_enabled)
	{

		/* render Nodes */

		JointsService* joints = JointsService::get();

		for (int i = 0; i < joints->offsetBodyNodes.size(); i++)
		{
			OffsetBodyNode* node = joints->offsetBodyNodes[i];
			if (node)
			{
				node->doRender(rd);
			}
		}

		/* -- render roblox stuff probably.. -- */
	}


}

void RBX::Diagnostics::Renderer::preRender(RenderDevice* rd)
{
	if (diagnostics_enabled)
	{
		/* render outlines for joint's primitives */

		if (joints_shouldRenderOutlines)
		{



		}
	}

}

void RBX::Diagnostics::Renderer::render2D(RenderDevice* rd)
{

}

void RBX::Diagnostics::Renderer::diag_jointsWeb(Gui::GuiButton* button)
{
	Diagnostics::get_Renderer()->joints_shouldRenderWebs = !
		Diagnostics::get_Renderer()->joints_shouldRenderWebs;
}

void RBX::Diagnostics::Renderer::diag_jointsOutline(Gui::GuiButton* button)
{
	Diagnostics::get_Renderer()->joints_shouldRenderOutlines = !
		Diagnostics::get_Renderer()->joints_shouldRenderOutlines;
}
