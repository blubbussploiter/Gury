#include "backpack.h"
#include "hopper.h"

#include "../Gury/Game/Network/Player/players.h"
#include "../Gury/Game/World/sounds.h"

static RBX::Sound* ping = RBX::Sound::fromFile("rbxasset://sounds\\electronicpingshort.wav");

void RBX::BackpackItem::drawOutline(RenderDevice* rd, Vector2 from, Vector2 to, float thickness, Color3 color)
{
	Vector2 p0, p1, p2, p3, p4, p5, p6, p7;

	p0 = from;
	p1 = Vector2(from.x+thickness, from.y+to.y);

	p2 = Vector2(from.x + to.x, from.y);
	p3 = Vector2(p2.x - thickness, from.y+to.y);

	p4 = from;
	p5 = Vector2(from.x + to.x, from.y + thickness);

	p6 = Vector2(from.x, from.y + to.y);
	p7 = Vector2(from.x + to.x, p6.y-thickness);

	Draw::box(Box(Vector3(p0.x, p0.y, 0), Vector3(p1.x, p1.y, 0)), rd, color, Color4::clear());
	Draw::box(Box(Vector3(p2.x, p2.y, 0), Vector3(p3.x, p3.y, 0)), rd, color, Color4::clear());
	
	Draw::box(Box(Vector3(p4.x, p4.y, 0), Vector3(p5.x, p5.y, 0)), rd, color, Color4::clear());
	Draw::box(Box(Vector3(p6.x, p6.y, 0), Vector3(p7.x, p7.y, 0)), rd, color, Color4::clear());

}

void RBX::BackpackItem::doActivate()
{
	RBX::HopperBin* item = getItem();
	RBX::Network::Player* player = RBX::Network::Players::get()->localPlayer;

	if (player) {

		RBX::HopperBin* activeBin;
		RBX::BackpackItem* active;

		activeBin = player->activeBin;
		ping->play();

		if (!player)
			return;

		if (!item->active)
		{
			if (player->activeBin)
			{
				active = Hopper::get()->getBackpackItem(player->activeBin);
				activeBin->deactivate();
				activeBin->active = 0;
			}

			player->activeBin = item;
			item->activate();
			item->active = 1;
		}
		else
		{
			activeBin->deactivate();
			player->activeBin = 0;
			item->active = 0;
		}

	}
}

void RBX::BackpackItem::getTextureId()
{
	textureId = item->getTextureId();

	if (!textureId.isEmpty)
	{
		if (textureId.resolve())
		{
			GImage textureImage = GImage(textureId.content, textureId.contentLength);
			if (textureImage.channels > 0) {
				texture->texture = Texture::fromGImage(name, textureImage);
			}
		}
	}
}

void RBX::BackpackItem::fromHopperBin(HopperBin* bin)
{
	item = bin;
	title = bin->getName();
	bin->onChanged.connect(doOnChanged);
	getTextureId();
}

RBX::BackpackItem::BackpackItem()
{
	alignBottom = true;

	texture = new RBX::Gui::GuiImage();
	outlineBox = new RBX::Gui::GuiBox();
	numberOutlineBox = new RBX::Gui::GuiBox();
	disabledBox = new RBX::Gui::GuiBox();

	number = new Gui::GuiLabel();

	size = Vector2(100, 100);
	outlineBox->size = Vector2(70, 70);
	texture->size = Vector2(70, 70);
	numberOutlineBox->size = Vector2(20, 20);
	number->sz = 10;
	sz = 10;

	number->title = "-1";

	outlineBox->background = Color4::clear();
	numberOutlineBox->background = Color3(0.8f, 0.8f, 0.8f);

	position = Vector2(0, 95);
	titleOff = Vector2(0, 50);

	number->textColor = Color3::white();
	titleColor = Color3::white();

	background = BACKPACK_ITEM_NEUTRAL_COLOR;
	clickedColor = BACKPACK_ITEM_CLICKED_COLOR;
	hoverColor = BACKPACK_ITEM_HOVER_COLOR;

	onClick = onClickFn;

	setName("BackpackItem");
	setClassName("BackpackItem");
}

void RBX::BackpackItem::render(RenderDevice* rd)
{
	RBX::Network::Player* player = RBX::Network::Players::get()->localPlayer;

	numberOutlineBox->position = position + Vector2(2, 80);
	number->position = numberOutlineBox->position + Vector2(5, 2);
	texture->position = position + Vector2(15, 15);

	outlineBox->position = texture->position;

	GuiButton::render(rd);

	if (player) {

		if (disabled) {
			disabled = false;
		}

		if (!hovered)
		{
			if (item->active)
			{
				drawOutline(rd, position, size, 2, Color3::green());
			}
			outlineBox->background = Color4::clear();
		}
		else
		{
			if (item->active)
			{
				drawOutline(rd, position, size, 4, Color3::green());
			}
			outlineBox->background = Color3::yellow();
		}

		if (clicked)
		{
			outlineBox->background = Color3::blue();
		}
		else
		{
			if (!hovered)
				outlineBox->background = Color4::clear();
		}

	}
	else { 
		if (!disabled) {
			disabled = true;
		}
	}

	numberOutlineBox->render(rd);
	number->render(rd);

	if (texture->texture.isNull())
	{
		title = item->getName();

		clickedColor = BACKPACK_ITEM_CLICKED_COLOR;
		hoverColor = BACKPACK_ITEM_HOVER_COLOR;
	}
	else
	{
		outlineBox->render(rd);
		texture->render(rd);

		hoverColor = background;
		clickedColor = background;

		disabledBox->background = Color4(1, 1, 1, 0.5f);
		disabledBox->position = texture->position;
		disabledBox->size = texture->size;

		if (disabled) {
			disabledBox->render(rd);
		}

		title.clear();
	}

}

void RBX::BackpackItem::doUpdateGridPosition(int backpackIndex)
{
	this->backpackIndex = backpackIndex;
	if (number) {
		number->title = std::to_string(backpackIndex + 1);
	}
	origin = Vector2(backpackIndex * 100, 95);
}

void RBX::BackpackItem::doOnChanged(Instance* hopperBin, std::string propertyName)
{
	HopperBin* bin = toInstance<HopperBin>(hopperBin);
	if (bin) {

		if (propertyName == "TextureId") {

			BackpackItem* item = Hopper::get()->getBackpackItem(bin);

			if (item) {
				item->getTextureId();
			}
		}

	}
}

void RBX::BackpackItem::onClickFn(RBX::Gui::GuiButton* b) /* move this to Backpack::activateBin() */
{
	RBX::BackpackItem* bitem = (RBX::BackpackItem*)(b);
	bitem->doActivate();
}

void RBX::BackpackItem::handleMouse(G3D::UserInput* ui)
{
	RBX::Gui::GuiButton::handleMouse(ui);
}
