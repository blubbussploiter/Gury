#include "hint.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Hint>("Hint")
         .constructor<>()
         .property("Text", &RBX::Hint::getText, &RBX::Hint::setText)(rttr::metadata("Type", RBX::Appearance));
}

void RBX::Hint::render2D(RenderDevice* renderDevice)
{
	Vector2 end;
	Vector2 bounds;

	GFontRef ref = Gui::get()->font;

	end = Vector2(renderDevice->width(), renderDevice->height());
	bounds = ref->get2DStringBounds(text);

	Draw::box(Box(Vector3(0, end.y - 20, 0), Vector3(end.x, end.y + 20, 0)), renderDevice, Color3::black(), Color4::CLEAR);
	ref->draw2D(renderDevice, text, Vector2((end.x / 2) - (bounds.x / 2), end.y - 18), 12, Color3::white(), Color3::black());
}
