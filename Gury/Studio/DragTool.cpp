
#include "../Gury/Game/Network/Player/Mouse.h"
#include "../Gury/Game/Services/selection.h"
#include "../Gury/Kernel/collisionTest.h"

#include "DragTool.h"

void RBX::Studio::DragTool::onDrag()
{
	SelectionService* selectionService;
	selectionService = SelectionService::get();
	Instances selection;
	selection = selectionService->selectionAsInstances();

	RBX::Mouse* mouse;
	mouse = RBX::Mouse::get();
	
	if (isDragging)
	{ 
		PVInstance* target = mouse->getTargetWithIgnoreList<Instance>(selection);
		selectionService->selectionAllowed = false;

		if (target)
		{
			PVInstance* rootPart = ModelInstance::getRootPart(selection);

			Vector3 hitSize = target->getSizeExternal();
			CoordinateFrame hitCoord = target->getCoordinateFrame();

			Vector3 hit = mouse->getHit();
			Vector3 hitOffset = Vector3(ceil(hit.x), 0, ceil(hit.z));

			Vector3 upVector = hitCoord.upVector();
			Vector3 upwardsPosition = (hitCoord.translation * normalize(upVector)) + (upVector * hitSize);

			Vector3 position = upwardsPosition + hitOffset;

			CoordinateFrame translated(hitCoord.rotation, position);

			if (selection.size() > 2)
			{
				ModelInstance::translateInstances(selection, rootPart, translated);
			}
			else
			{
				PVInstance* pvInstance = toInstance<PVInstance>(selection.at(0));
				pvInstance->setCFrame(translated);
			}
		}

	}
	else
	{
		selectionService->selectionAllowed = true;
	}
}

void RBX::Studio::DragTool::doLogic(UserInput* userInput)
{
	SelectionService* selectionService;
	selectionService = SelectionService::get();

	Instances selection = selectionService->selection;

	if (userInput->keyDown(SDL_LEFT_MOUSE_KEY) &&
		selection.size() > 0)
	{
		if (justReleased)
		{
			justReleased = false;
		}
		else
		{
			isDragging = true;
		}
	}

	if (userInput->keyReleased(SDL_LEFT_MOUSE_KEY))
	{
		selection.clear();
		isDragging = false;
		justReleased = true;
	}

	onDrag();
}
