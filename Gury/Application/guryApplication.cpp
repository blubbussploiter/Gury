
#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/StudioTool.h"
#include "../Gury/Studio/MainFrm.h"
#include "../Studio/ChildFrm.h"

#include "../Gury/Game/Services/selection.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Services/hopper.h"

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
	if (Datamodel::get() != nullptr)
	{

		if (userInput->keyPressed(SDLK_o))
		{
			RBX::Camera::get()->cam_zoom(0);
		}
		if (userInput->keyPressed(SDLK_i))
		{
			RBX::Camera::get()->cam_zoom(1);
		}

		if (RBX::Studio::current_Tool)
		{
			RBX::Studio::current_Tool->doLogic(userInput);
		}

		RBX::Studio::StudioNavigator::get()->doActions(userInput);
		RBX::Hopper::get()->doUserInput(userInput);

		Mouse::get()->doUserInput(userInput);

		Camera* cam = getCamera();
		if (cam)
		{
			cam->update(inGuryWindow() && userInput->keyDown(SDL_RIGHT_MOUSE_KEY));
		}

		bool overGui = Gui::get()->doButtonLogic(userInput, renderDevice);
		if (overGui) {
			RBX::SelectionService::get()->update(userInput);
		}

		Mouse::get()->updateCursorInfo(overGui);

		ControllerService::get()->updateControllers(userInput);

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
	/* Change to true if you want to enable diagnostics */
	//Diagnostics::get_Renderer()->diagnostics_enabled = true;

	updateAppName();

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
	CMainFrame* mainFrame = CMainFrame::mainFrame;

	if (mainFrame)
	{

		inEditMode = _inEditMode;
		int n = inEditMode ? SW_RESTORE : SW_HIDE;

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

		}
		else
		{

		}
	}
}

void RBX::Experimental::Application::mainProcessStep()
{

	if (datamodel != nullptr && datamodel->loaded)
	{
		if (applicationThinking)
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

}

void RBX::Experimental::Application::start()
{
	if (!isInitialized)
	{
		RBX::StandardOut::print(RBX::MESSAGE_INFO, "Application::start()");

		isInitialized = true;
		applicationThinking = true;

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
