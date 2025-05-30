#include "GuiRoot.h"

void RBX::Gui::GuiMessage::render(RenderDevice* d)
{
	Vector2 end;
	Vector2 bounds;

	if (!text.empty())
	{
		GFontRef ref = Gui::get()->font;

		end = Vector2(d->width(), d->height());
		bounds = ref->get2DStringBounds(text);

		Draw::box(Box(Vector3(0, end.y - 20, 0), Vector3(end.x, end.y, 0)), d, Color3::black(), Color4::CLEAR);

		ref->draw2D(d, text, Vector2((end.x / 2) - (bounds.x / 2), end.y - 20), 12, Color3::white(), Color3::black());
	}
}