#include "GuiRoot.h"

#include "../Gury/Game/World/sounds.h"
#include "../Gury/Application/appmanager.h"

void RBX::Gui::CameraPanMenu::onArrowUpBtnClick(GuiButton* btn)
{
	Camera::get()->tiltDown();
	Camera::switch3->play();
}

void RBX::Gui::CameraPanMenu::onArrowDwnBtnClick(GuiButton* btn)
{
	Camera::get()->tiltUp();
	Camera::switch3->play();
}

void RBX::Gui::CameraPanMenu::onZoomOutBtnClick(GuiButton* btn)
{
	Camera::get()->cam_zoom(0);
	Camera::switch3->play();
}

void RBX::Gui::CameraPanMenu::onZoomInBtnClick(GuiButton* btn)
{
	Camera::get()->cam_zoom(1);
	Camera::switch3->play();
}
