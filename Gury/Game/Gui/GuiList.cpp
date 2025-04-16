#include "GuiRoot.h"

void RBX::Gui::GuiList::render(RenderDevice* d)
{
	Vector2 end;
	Vector2 pos;

	size = Vector2(size.x, startY+children->size() * 15);
	if (!visible) return;

	if (!alignRight)
	{
		end = (position + size);
		pos = position;
	}
	else
	{
		float spacing = (position.x / 2) + (size.x / 2);
		pos = Vector2(d->width() - spacing * 1.10, position.y);
		end = (pos + size);
	}

	for (unsigned int i = 0; i < children->size(); i++)
	{
		GuiObject* object0 = toInstance<GuiObject>(children->at(i));
		Vector2 _pos = pos;

		if (i != 0) /* NOT first child */
		{
			GuiObject* object1 = toInstance<GuiObject>(children->at(i - 1));
			_pos = object1->position + Vector2(0, 12);
		}

		if (!i && !listTitle.empty())
		{
			_pos += Vector2(0, 18);
		}

		object0->position = _pos;
		object0->render(d);
	}

	Draw::box(Box(Vector3(pos.x, pos.y, 0), Vector3(end.x, end.y, 0)), d, Color4(0.5f, 0.5f, 0.5f, 0.3f), Color4::CLEAR);

	if (!listTitle.empty())
	{
		get()->font->draw2D(d, listTitle, pos, 10, Color3::white(), Color4::CLEAR);
		if (!appliedTitle)
		{
			startY = 20;
			appliedTitle = true;
		}
	}
}

void RBX::Gui::GuiList::addChild(GuiObject* o)
{
	o->setParent(this);
}

void RBX::Gui::GuiList::removeChild(GuiObject* o)
{
	o->remove();
}
