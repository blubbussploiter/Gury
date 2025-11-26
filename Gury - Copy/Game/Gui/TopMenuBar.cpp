
#include "../Gury/Game/rbxwindow.h"
#include "../Gury/Application/appmanager.h"

#include "GuiRoot.h"

bool inFullScreen = 0;
int x, y, width, height;

void setFullScreen(bool s)
{

}

void RBX::Gui::MenuBar::onExitBtnClick(RBX::Gui::GuiButton* btn)
{
	RBX::RBXManager::get()->getApplication()->sendClose();
}

void RBX::Gui::MenuBar::onFullscreenBtnClick(GuiButton* btn)
{

	if (!inFullScreen)
	{
		inFullScreen = 1;
		btn->title = "x Fullscreen";
	}
	else
	{
		inFullScreen = 0;
		btn->title = "Fullscreen";
	}

	setFullScreen(inFullScreen);

}

void RBX::Gui::MenuBar::onEditModeBtnClick(GuiButton* btn)
{
	RBX::Experimental::Application* app;
	app = RBX::RBXManager::get()->getApplication();

	app->exitEditMode(!app->inEditMode);

	if (!app->inEditMode)
	{
		btn->title = "Edit Mode";
	}
	else
	{
		btn->title = "x Edit Mode";
	}
}
