
#include "../Gury/Game/Network/Player/Mouse.h"
#include "../Gury/Game/Services/selection.h"

#include "GuiRoot.h"

RBX::Gui::GuiRoot* root;

Vector2 RBX::Gui::getPosition(float width, float height, Vector2 position, bool alignRight, bool alignBottomRight, bool alignBottom)
{

	if (alignBottomRight)
		return Vector2(width - position.x, height - position.y);

	if (alignBottom)
		return Vector2(position.x, height-position.y);

	return position;
}

RBX::Gui::GuiRoot* RBX::Gui::get()
{
	return Datamodel::get()->guiRoot;
}

void RBX::Gui::GuiRoot::render(RenderDevice* d)
{
	for (unsigned int i = 0; i < children->size(); i++)
	{
		GuiObject* object = toInstance<GuiObject>(children->at(i));
		if (object)
		{
			object->render(d);
		}
	}
}

void RBX::Gui::GuiRoot::doButtonLogic(G3D::UserInput* ui, RenderDevice* d)
{
	for (unsigned int i = 0; i < children->size(); i++)
	{
		GuiObject* object = toInstance<GuiObject>(children->at(i));
		if (object)
		{
			object->handleMouse(ui);
		}
	}
}

bool RBX::Gui::GuiObject::mouseIn(Vector2 mouse)
{
	return (mouse.x >= position.x && mouse.y >= position.y && (mouse.x < position.x + size.x && mouse.y < position.y + size.y));
}

void RBX::Gui::GuiObject::handleMouse(UserInput* ui)
{
}
