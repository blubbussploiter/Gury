
#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/StudioTool.h"
#include "../Gury/Studio/MainFrm.h"

#include "../Gury/Game/Services/selection.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/Network/Player/Mouse.h"

#include "../Gury/Game/Network/networkclient.h"
#include "../Gury/Game/Services/scriptcontext.h"

#include "../Gury/Game/Services/soundservice.h"
#include "../Gury/Game/Rendering/diagnosticsWorldDrawer.h"

#include "../Gury/Game/globalSounds.h"

#include "appmanager.h"

RBX::Datamodel* RBX::Experimental::Application::getDatamodel()
{
	if (!datamodel)
	{
		datamodel = new RBX::Datamodel();
		datamodel->open();
	}
	return datamodel;
}

RBX::Camera* RBX::Experimental::Application::getCamera()
{
	return Workspace::get()->getCurrentCamera();
}

void RBX::Experimental::Application::doUserInput()
{
	GEvent e;

	userInput->beginEvents();

	while (pollEvent(mouse, e) || pollEvent(key, e))
	{
		if (mouse.message != 0)
		{
			mouse.message = 0;
		}

		if (key.message != 0)
		{
			key.message = 0;
		}

		userInput->processEvent(e);
	}

	window->pollEvent(e);

	userInput->endEvents();
}

void RBX::Experimental::Application::onSimulation(RealTime rdt, SimTime sdt, SimTime idt)
{
	getDatamodel()->step(idt);
	SoundService::get()->update();
}

void RBX::Experimental::Application::onLogic()
{
	if (datamodel != nullptr)
	{

		if (userInput->keyPressed(SDLK_o))
		{
			RBX::Camera::get()->cam_zoom(0);
		}
		if (userInput->keyPressed(SDLK_i))
		{
			RBX::Camera::get()->cam_zoom(1);
		}

		if (userInput->keyPressed(SDLK_DELETE))
		{
			GlobalSounds::pageTurn->play();
			RBX::SelectionService::get()->removeSelected();
		}

		if (RBX::Studio::current_Tool)
		{
			RBX::Studio::current_Tool->doLogic(userInput);
		}

		Mouse::get()->update(userInput);
		getCamera()->update(userInput->keyDown(SDL_RIGHT_MOUSE_KEY));

		RBX::Gui::get()->doButtonLogic(userInput, renderDevice);
		RBX::Network::Players::get()->onStep();

		RBX::ControllerService::get()->updateControllers(userInput);
		RBX::SelectionService::get()->update(userInput);

	}

}

void RBX::Experimental::Application::onFocus()
{
	justReceivedFocus = 1;
}

void RBX::Experimental::Application::onKillFocus()
{
	justReceivedFocus = 0;

	mouse.message = 0;
	mouse.wParam = 0;
	mouse.lParam = 0;

	key.message = 0;
	key.wParam = 0;
	key.lParam = 0;
}

void RBX::Experimental::Application::updateAppName() {
	appName = RBXManager::get()->getDocumentCaption(this);
}

void RBX::Experimental::Application::onGraphics()
{
	RBX::Render::RenderScene::get()->oneFrame(renderDevice, getCamera(), sky);
}

void RBX::Experimental::Application::onInit()
{
	Diagnostics::get_Renderer()->diagnostics_enabled = false;

	updateAppName();

	RBX::StandardOut::print(RBX::MESSAGE_INFO, "Application::onInit()");
	RBX::RBXManager::get()->initOneTimeAppliances();

	setWindowLong();

	getDatamodel();

	RBX::ControllerService::get()->mapControllers(userInput);
	RBX::ControllerService::get()->addController(RBX::Camera::get());
	
	RBX::ScriptContext* context = getDatamodel()->scriptContext;
	context->openState();

	getCamera();
	
	inEditMode = true;

	if (!rbxlFile.empty())
	{
		getDatamodel()->name = RBX::RBXManager::get()->fileName;
		getDatamodel()->loadContent(rbxlFile);
	}

}

void RBX::Experimental::Application::exitEditMode(bool _inEditMode)
{
	int n;
	CMainFrame* mainFrame = CMainFrame::mainFrame;

	if (!mainFrame) return;

	inEditMode = _inEditMode;
	n = inEditMode ? SW_RESTORE : SW_HIDE;

	mainFrame->m_wndMainTools.ShowWindow(n);
	mainFrame->m_wndCameraTools.ShowWindow(n);
	mainFrame->m_wndClassView.ShowWindow(n);
	mainFrame->m_wndStudioTools.ShowWindow(n);
	mainFrame->m_wndRunServiceTools.ShowWindow(n);
	mainFrame->m_wndProperties.ShowWindow(n);
	mainFrame->m_wndOutput.ShowWindow(n);
	mainFrame->m_wndMenuBar.ShowWindow(n);
	mainFrame->m_wndStatusBar.ShowWindow(n);

	if (inEditMode)
	{
		ShowWindow(viewHwnd, SW_RESTORE);
	}
	else
	{
		ShowWindow(viewHwnd, SW_MAXIMIZE);
	}

	resizeWithParent();
}

void RBX::Experimental::Application::mainProcessStep()
{

	if (datamodel != nullptr 
		&& isThinking)
	{
		RealTime desiredFrameDuration = 1.0 / fps;
		RealTime now = System::time();

		lastTime = now;
		RealTime timeStep = now - lastTime;
		
		doUserInput();

		onLogic();

		onGraphics();

		double rate = simTimeRate;
		RealTime rdt = timeStep;
		SimTime  sdt = timeStep * rate;
		SimTime  idt = desiredFrameDuration * rate;

		onSimulation(rdt, sdt, idt);

		now = System::time();

		lastWaitTime = System::time();
	}

}

void RBX::Experimental::Application::start()
{
	if (!isInitialized)
	{
		RBX::StandardOut::print(RBX::MESSAGE_INFO, "Application::start()");

		isInitialized = true;
		isThinking = true;

		onInit();
	}
}

void RBX::Experimental::Application::sendClose()
{
	if (IsWindow(viewHwnd))
	{
		SendMessage(viewHwnd, WM_CLOSE, 0, 0);
	}
}

void RBX::Experimental::Application::close()
{
	RBX::Datamodel::get()->close();
	renderDevice->cleanup();
}
