
#include "../Gury/Game/Services/selection.h"
#include "../Gury/Kernel/collisionTest.h"

#include "MoveTool.h"

void RBX::Studio::MoveTool::onMove()
{

}

void RBX::Studio::MoveTool::doGraphics(RenderDevice* rd)
{
	SelectionService* selectionService;
	selectionService = SelectionService::get();
	Instances selection = selectionService->selectionAsInstances();

	if (selection.size() == 1)
	{
		return;
	}


}

void RBX::Studio::MoveTool::doLogic(UserInput* userInput)
{

}
