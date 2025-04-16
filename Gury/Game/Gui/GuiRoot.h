#pragma once

#include <G3DAll.h>
#include <vector>

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/strings.h"

namespace RBX
{
	namespace Gui
	{

		/* Calling :remove or ->remove on any of these will crash ur game ok ty */

		Vector2 getPosition(float width, float height, Vector2 pos, bool alignRight, bool alignBottomRight, bool alignBottom);

		class GuiObject : public RBX::Instance
		{
		public:

			bool alignRight;
			bool alignBottomRight;
			bool alignBottom;

			Vector2 origin;
			Vector2 position;
			Vector2 size;

			bool visible;
			bool isButton;

			bool mouseIn(Vector2 mouse);

			virtual void handleMouse(UserInput* ui);

			virtual void render(RenderDevice* d) {}

			GuiObject() : visible(true) {
				alignBottom = 0;
				alignRight = 0;
				alignBottomRight = 0;
				setClassName("GuiObject"); 
				setName("GuiObject");
			}
		};

		class GuiBox : public GuiObject
		{
		public:
			Color4 background;
			Color4 outline;

			void render(RenderDevice* d);

			GuiBox() {background = Color4(0.5f, 0.5f, 0.5f, 0.3f); outline = Color4::CLEAR;}
		};

		class GuiList : public GuiObject
		{
		public:

			std::string listTitle;
			float startY;

			bool appliedTitle;

			void addChild(GuiObject* o);
			void removeChild(GuiObject* o);

			void render(RenderDevice* d);
		};

		class GuiButton : public GuiBox
		{
		public:

			std::string title;

			Color4 titleColor, hoverColor, disabledColor, clickedColor, renderColor;
			Vector2 titleOff;

			float sz;

			bool disabled;
			bool hovered;

			bool clicked;

			void(*onClick)(GuiButton* b);

			void render(RenderDevice* d);
			void handleMouse(UserInput* ui);

			GuiButton() {
				isButton = true; sz = 12.f; 
				titleColor = Color4(0.25f, 0.25f, 0.25f, 0.95f);
				hoverColor = Color4(0.6f, 0.6f, 0.6f, 0.3f);
				disabledColor = Color4(0.6f, 0.6f, 0.6f, 0.6f);
				renderColor = Color4(0.5f, 0.5f, 0.5f, 0.3f);;
				clickedColor = hoverColor;
			}
		};

		class GuiImage : public GuiObject
		{
		public:
			TextureRef texture;
			int width;
			int height;

			void render(RenderDevice* d);
		};

		class GuiImageButton : public GuiButton
		{
		public:

			TextureRef texture;
			TextureRef hoverTexture;
			TextureRef clickTexture;

			int width;
			int height;

			void render(RenderDevice* d);
			void handleMouse(G3D::UserInput* ui);

		};

		class GuiLabel : public GuiObject
		{
		public:
			float sz;
			std::string title;

			Color4 textColor;
			Color4 outlineColor;

			void render(RenderDevice* d);
			GuiLabel() : title("GuiLabel"), sz(12.f), textColor(Color3::black()), outlineColor(Color4::CLEAR) {}
		};

		class GuiMessage : public GuiObject
		{
		public:
			std::string text;

			void render(RenderDevice* d);

		};

		class GuiHint : public GuiObject
		{
		public:
			std::string text;

			void render(RenderDevice* d);

		};

		class GuiRoot : public Instance
		{
		public:

			GFontRef font;

			void add(GuiObject* obj) { obj->setParent(this); }

			void render(RenderDevice* d);

			bool doButtonLogic(G3D::UserInput* ui, RenderDevice* d);

			void initFont()
			{
				if (font.isNull())
				{
					font = GFont::fromFile(GetFileInPath("/content/font/lighttrek-small.fnt"));
				}
			}

			GuiRoot() 
			{ 

			}
		};

		class CameraPanMenu
		{
		public:
			static void onArrowUpBtnClick(GuiButton* btn);
			static void onArrowDwnBtnClick(GuiButton* btn);
			static void onZoomInBtnClick(GuiButton* btn);
			static void onZoomOutBtnClick(GuiButton* btn);
		};

		class MenuBar
		{
		public:
			static void onExitBtnClick(GuiButton* btn);
			static void onFullscreenBtnClick(GuiButton* btn);
			static void onEditModeBtnClick(GuiButton* btn);
		};

		/* deprecated, use Datamodel->guiRoot */
		GuiRoot* get(); 
	}
}
