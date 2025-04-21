#pragma once

#include "content.h"
#include "../Gury/Game/Gui/GuiRoot.h"
#include "../Gury/Game/Objects/Bins/hopperbin.h"

#include <map>

#define BACKPACK_ITEM_HOVER_COLOR  Color4(0.7f, 0.7f, 0.7f, 1.f);
#define BACKPACK_ITEM_CLICKED_COLOR Color3::yellow();
#define BACKPACK_ITEM_NEUTRAL_COLOR Color4(0.7f, 0.7f, 0.7f, .5f);

namespace RBX
{
	class BackpackItem;
	class HopperBin;


	class BackpackItem : public RBX::Gui::GuiButton
	{
	private:
		HopperBin* item;
	public:
		int backpackIndex;

		Gui::GuiBox* outlineBox;
		Gui::GuiBox* numberOutlineBox;
		Gui::GuiBox* disabledBox;
		Gui::GuiImage* texture;
		Gui::GuiLabel* number;
	public:

		Content textureId;

		void doActivate();

		HopperBin* getItem() { return item; }
		void getTextureId();

		void fromHopperBin(HopperBin* bin);

		void drawOutline(RenderDevice* rd, Vector2 from, Vector2 to, float thickness, Color3 color);
		void render(RenderDevice* rd);

		void doUpdateGridPosition(int backpackIndex);

		static void doOnChanged(Instance* hopperBin, std::string propertyName);
		static void onClickFn(RBX::Gui::GuiButton* b);
		void handleMouse(G3D::UserInput* ui);
		
		BackpackItem();
	};

	class Backpack : public RBX::Instance
	{
	public:

		static void doOnChildAdded(Instance* _this, Instance* child);
		static void doOnChildRemoved(Instance* _this, Instance* child);

		Backpack()
		{
			setName("Backpack");
			setClassName("Backpack");

			onChildAdded.connect(doOnChildAdded);
			onChildRemoved.connect(doOnChildRemoved);
		}
		virtual ~Backpack() {}

		RTTR_ENABLE(RBX::Instance)
	};

}
