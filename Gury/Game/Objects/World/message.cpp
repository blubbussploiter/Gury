
#include "../Gury/Game/Network/Player/players.h"

#include "message.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Message>("Message")
         .constructor<>()
         .property("Text", &RBX::Message::getText, &RBX::Message::setText)(rttr::metadata("Type", RBX::Appearance));
}

void RBX::Message::renderFullScreen(RenderDevice* renderDevice)
{
	Vector2 end;
	Vector2 textBounds;

	GFontRef ref = Gui::get()->font;

	end = Vector2(renderDevice->width(), renderDevice->height());
	textBounds = ref->get2DStringBounds(text);

	Draw::box(Box(Vector3(0, 0, 0), Vector3(end.x, end.y, 0)), renderDevice, Color4(0.5f, 0.5f, 0.5f, 0.3f), Color4::CLEAR);

	ref->draw2D(renderDevice, text, Vector2((end.x / 2) - (textBounds.x / 2), end.y / 2), 12.f, Color3::white(), Color3::black());
}

void RBX::Message::renderPersonalMsg(RenderDevice* renderDevice)
{
	Rect2D viewport = renderDevice->getViewport();

	Vector2 translucentAreaLow = Vector2(
		0.25f * viewport.x0() + 30.0f,
		viewport.y0() + 30.0f);
	Vector2 translucentAreaHigh = Vector2(
		viewport.x1() * 0.25f + 30.0f,
		viewport.y1() * 0.25f + 30.0f);

	Vector2 textBounds;
	GFontRef ref = Gui::get()->font;

	Draw::box(Box(Vector3(translucentAreaLow.x, translucentAreaLow.y, 0), Vector3(translucentAreaHigh.x, translucentAreaHigh.y, 0)), renderDevice, Color4(0.5f, 0.5f, 0.5f, 0.5f), Color4::CLEAR);

	textBounds = ref->get2DStringBounds(text);

	ref->draw2D(renderDevice, text, Vector2((translucentAreaLow.x + translucentAreaHigh.x) / 2 - (textBounds.x / 2), (translucentAreaLow.y + translucentAreaHigh.y) / 2), 12.f, Color3::white(), Color3::black());
}

void RBX::Message::render2D(RenderDevice* renderDevice)
{
	Network::Player* localPlayer = Network::Players::get()->getLocalPlayer();

	if (!text.empty()) {

		if (parent == localPlayer) {
			renderPersonalMsg(renderDevice);
		}
		else
		{
			renderFullScreen(renderDevice);
		}

	}
}
