
/* gury gury */

#include "../Gury/Game/Network/Player/Mouse.h"
#include "../Gury/Game/Rendering/scene.h"

#include "selection.h"
#include "stdout.h"

/* mfc stuff */

#include "../Gury/Studio/pch.h"

#include "../Gury/Main/framework.h"
#include "../Gury/Studio/MainFrm.h"

#include "../Gury/Studio/StudioTool.h"

#include "../Gury/Game/Objects/model.h"

bool RBX::SelectionService::isSelected(Instance* i)
{
	auto b = selection.begin(), e = selection.end();
	return (std::find(b, e, i) != e);
}

RBX::Instances RBX::SelectionService::selectionAsInstances()
{
	return selection;
}

RBX::ISelectableArray RBX::SelectionService::instancesAsSelection(Instances instances)
{
	RBX::ISelectableArray selectable;
	for (unsigned int i = 0; i < instances.size(); i++)
	{
		ISelectable* s = dynamic_cast<ISelectable*>(instances.at(i));
		if (s)
		{
			selectable.push_back(s);
		}
	}
	return selectable;
}

void RBX::SelectionService::dragSelect()
{
	Vector2 a1(min(worldSelectStart.x, worldSelectEnd.x), min(worldSelectStart.y, worldSelectEnd.y));
	Vector2 a2(max(worldSelectStart.x, worldSelectEnd.x), max(worldSelectStart.y, worldSelectEnd.y));

	Instances instances;
	instances = RBX::Scene::get()->getArrayOfObjects();

	for (unsigned int i = 0; i < instances.size(); i++)
	{
		RBX::Instance* instance = dynamic_cast<RBX::Instance*>(instances.at(i));
		RBX::PVInstance* child = toInstance<PVInstance>(instance);

		if (child)
		{
			CoordinateFrame cframe = Camera::get()->getCoordinateFrame();
			Vector3 objectSpace = cframe.pointToObjectSpace(child->getPosition());
			
			float x = objectSpace.x / -objectSpace.z;
			float y = objectSpace.y / -objectSpace.z;

			if ((a1.x < x && x < a2.x) && (a1.y < y && y < a2.y) && objectSpace.z < 0)
			{
				doSelect(child, 1);
			}
		}
	}
}

void RBX::SelectionService::renderDragBox(RenderDevice* rd)
{
	if (down)
	{
		Vector3 start = selectionDragBoxStart;
		Vector3 end = selectionDragBoxEnd;

		Studio::StudioTool* currentTool;

		currentTool = Studio::current_Tool;

		if (selectionDragBoxEnd != Vector3::zero() && selectionDragBoxStart != Vector3::zero())
		{
			if (!currentTool || (currentTool && !currentTool->isUsing))
			{
				//Draw::box(Box(start, end), rd, Color4::clear(), Color3::gray());
			}
		}
	}
}

void RBX::SelectionService::renderSelected(RenderDevice* rd, ISelectable* selection)
{
	Render::Geometry box;
	box = selection->getBoundingBox();

	rd->pushState();

	rd->setObjectToWorldMatrix(box.cframe);
	Primitives::drawOutline(rd, -box.size, box.size, Color3::cyan(), 0.5f);

	rd->popState();
}

void RBX::SelectionService::renderSelection(RenderDevice* rd)
{
	Workspace* workspace = Workspace::get();
	for (RBX::Instance* pv : selection)
	{
		RBX::ISelectable* s = toInstance<RBX::ISelectable>(pv);
		if (workspace->isAncestorOf(pv) && s && s != workspace)
		{
			renderSelected(rd, s);
		}
	}
}

void RBX::SelectionService::removeSelected()
{
	for (RBX::Instance* i : selection)
	{
		i->remove();
	}
}

RBX::Instance* RBX::SelectionService::getPossibleSelectedItem()
{
	RBX::Instance* selected = 0;

	RBX::Camera* camera = RBX::Camera::get();
	Vector2 mousePosition = Vector2(Mouse::get()->x, Mouse::get()->y);

	Ray ray = camera->camera->worldRay(mousePosition.x, mousePosition.y, RBX::RBXManager::get()->getApplication()->getViewport());
	Instances* instances = Workspace::get()->children;

	float nearest = inf();
	size_t locked = 0;

	for (unsigned int i = 0; i < instances->size(); i++)
	{
		RBX::Instance* instance = instances->at(i);
		RBX::ISelectable* selectable = toInstance<ISelectable>(instance);

		if (selectable)
		{
			Render::Geometry geom = selectable->getBoundingBox();

			if (instance)
			{

				if (IsA<PVInstance>(instance)) /* Check if locked */
				{
					PVInstance* pvInstance = toInstance<PVInstance>(instance);
					if (pvInstance->getLocked()) {
						continue;
					}
				}

				/* Comment this out if you want just by part selection */

				if (IsA<ModelInstance>(instance)) /* Check if model locked */
				{
					ModelInstance* modelInstance = toInstance<ModelInstance>(instance);
					if (modelInstance->isLocked()) {
						continue;
					}
				}

			}

			if (geom.size == Vector3::zero()) {
				continue;
			}

			Box b;
			b = geom.getBox();

			float newdistance = ray.intersectionTime(b);

			if (G3D::isFinite(newdistance) && nearest > abs(newdistance))
			{
				nearest = newdistance;
				selected = instance;
			}
		}
	}

	return selected;
}

void RBX::SelectionService::update(UserInput* ui)
{
	if (Camera::get())
	{

		bool ctrlShift = ui->keyDown(SDLK_RSHIFT) || ui->keyDown(SDLK_LSHIFT)
			|| ui->keyDown(SDLK_RCTRL) || ui->keyDown(SDLK_LCTRL);

		clicked = ui->keyPressed(SDL_LEFT_MOUSE_KEY);
		down = ui->keyDown(SDL_LEFT_MOUSE_KEY);

		multiSelect = (down && !clicked || ctrlShift);

		if (selectionAllowed)
		{
			CoordinateFrame cframe;

			Studio::StudioTool* currentTool;
			currentTool = Studio::current_Tool;

			Instance* target = getPossibleSelectedItem();
			cframe = Camera::get()->getCoordinateFrame();

			if (down)
			{
				Vector3 rel = cframe.pointToObjectSpace(Mouse::get()->getHit());
				worldSelectEnd = Vector2(rel.x / -rel.z, rel.y / -rel.z);
				selectionDragBoxEnd = ui->getMouseXY();
			}
			if (clicked)
			{
				Vector3 rel = cframe.pointToObjectSpace(Mouse::get()->getHit());
				worldSelectStart = Vector2(rel.x / -rel.z, rel.y / -rel.z);
				selectionDragBoxStart = ui->getMouseXY();
			}

			if (!currentTool || (currentTool && !currentTool->isUsing))
			{
				//dragSelect();
			}

			if (clicked)
			{
				if (doSelect(target, multiSelect))
				{
					return;
				}
				CMainFrame::mainFrame->m_wndClassView.SelectInstance(0);
				selection.clear();
			}
		}
	}

}

bool RBX::SelectionService::doSelect(Instance* target, bool multiSelect)
{
	if (target)
	{
		PVInstance* pv = toInstance<PVInstance>(target);
		if (pv && pv->locked) { 
			return 0;
		}
		if (!isSelected(target))
		{
			if (!multiSelect)
			{
				selection.clear();
			}
			selectInstance(target);
		}
		return 1;
	}
	return 0;
}

void RBX::SelectionService::selectArray(Instances instances)
{
	for (size_t i = 0; i < instances.size(); i++)
	{
		selectInstance(instances.at(i));
	}
}

void RBX::SelectionService::deselectAll()
{
	selection.clear();
	CMainFrame::mainFrame->m_wndClassView.SelectInstance(0);
}

void RBX::SelectionService::selectInstance(RBX::Instance* instance, bool selectInExplorer)
{
	if (std::find(selection.begin(), selection.end(), instance) == selection.end())
	{
		if (instance && selectInExplorer)
		{
			CMainFrame::mainFrame->m_wndClassView.SelectInstance(instance);
		}
		selection.push_back(instance);
	}
}

RBX::SelectionService* RBX::SelectionService::get()
{
	return Datamodel::get()->selectionService;
}