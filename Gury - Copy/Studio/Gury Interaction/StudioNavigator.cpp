#include "../StudioTool.h"

#include "..\Gury\Game\Services\selection.h"
#include "..\Gury\Game\globalSounds.h"

RBX::Studio::StudioNavigator* navigator = new RBX::Studio::StudioNavigator();

void RBX::Studio::StudioNavigator::bindAction(SDLKey hotKey, std::function<void()> action)
{
}

void RBX::Studio::StudioNavigator::doActions(UserInput* userInput)
{
	Experimental::Application* application = RBXManager::get()->getApplication();
	if (application->applicationThinking) {

	}
}

void RBX::Studio::StudioNavigator::removeSelected()
{
	GlobalSounds::pageTurn->play();
	RBX::SelectionService::get()->removeSelected();
}

RBX::Studio::StudioNavigator* RBX::Studio::StudioNavigator::get()
{
	return navigator;
}
