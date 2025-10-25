#pragma once

#include <stack>

#ifndef BUILDWXP
#include <afxwin.h>
#endif

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Objects/model.h"

#include "../Gury/Game/Services/stdout.h"

namespace RBX
{
	namespace Experimental
	{

		class Application
		{
		private:

			bool isInitialized;
			RealTime lastWaitTime, lastTime;

			Datamodel* datamodel;

			Array<CWnd*> editoralHiddenWindows;
			Camera* camera; /* main viewport */

			bool                 _mouseButtons[3];
			bool                 _keyboardButtons[0xFF];

			double simTimeRate = 1.0;

		public:

			HWND viewHwnd;

			SkyRef sky;

			bool applicationThinking;
			bool justReceivedFocus;
			bool inEditMode;

			int width, height;
			float fps;

			WNDPROC wndProc;
			MSG key, mouse; /* jank but it works :D */

			RenderDevice* renderDevice;
			G3D::UserInput* userInput;

			GWindow* window;

			std::string rbxlFile;
			std::string appName;

			/* Window stuff */

			void setWindowLong();
			void resizeWithParent(int cx=0, int cy=0);
			void resize(int cx, int cy);

			bool inGuryWindow();

			/* onXXX */

			void onGraphics();
			void onLogic();
			void onInit();

			void exitEditMode(bool inEditMode=false);

			void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);
			void doUserInput();

			bool pollEvent(MSG msg, GEvent& e);

			/* Application stuff */

			void mainProcessStep();
			void start();

			void sendClose();
			void close();

			Datamodel* getDatamodel();
			Camera* getCamera();

			void onFocus();
			void onKillFocus();

			Rect2D getViewport() {
				return renderDevice->getViewport();
			}

			void updateAppName();

			void suspend() {
				applicationThinking = false;
			}

			void resume() {
				applicationThinking = true;
			}

			bool initialized() { return isInitialized; }

			Application(HWND wnd);
		};
	}

	extern SkyRef getGlobalSky();
}