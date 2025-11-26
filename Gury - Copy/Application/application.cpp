
#include <thread>

#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Network/Player/Mouse.h"

#include "appmanager.h"

BOOL CALLBACK documentWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RBX::Experimental::Application* app = RBX::RBXManager::get()->getApplication();

	if (app)
	{
		switch (uMsg)
		{
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			{
				app->mouse.hwnd = hwnd;
				app->mouse.message = uMsg;
				app->mouse.wParam = wParam;
				app->mouse.lParam = lParam;
				break;
			}
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				app->key.hwnd = hwnd;
				app->key.message = uMsg;
				app->key.wParam = wParam;
				app->key.lParam = lParam;
				break;
			}
			case WM_CLOSE: /* Maybe this works? */
			{
				WNDPROC oldWndProc = app->wndProc;
				app->wndProc = 0;
				app->close(); /* Close datamodel */
				return CallWindowProcA(oldWndProc, hwnd, uMsg, wParam, lParam);
			}
			default: break;
		}

		if (app->wndProc)/* so its crashing here.. not good */
		{/* put gury in the view instead of the frame, so much bullshit happening cos of it in the frame window */
			return CallWindowProcA(app->wndProc, hwnd, uMsg, wParam, lParam);
		}
	}
	return 0;
}

void RBX::Experimental::Application::setWindowLong()
{
	if (!wndProc)
	{
		wndProc = (WNDPROC)SetWindowLongPtr(viewHwnd, GWL_WNDPROC, (LONG_PTR)&documentWndProc);
	}
}

void RBX::Experimental::Application::resize(int cx, int cy)
{
	RECT r;
	GetClientRect(viewHwnd, &r);

	Rect2D viewportRect = Rect2D::xywh(r.left, r.top, cx, cy);
	renderDevice->notifyResize(cx, cy);
	renderDevice->setViewport(viewportRect);
}

bool RBX::Experimental::Application::inGuryWindow()
{
	float x = Mouse::get()->x;
	float y = Mouse::get()->y;

	RECT rect;
	GetClientRect(viewHwnd, &rect);

	return (x >= rect.left && y >= rect.top &&
		x < rect.right && y < rect.bottom) && (GetFocus() == viewHwnd);
}

void RBX::Experimental::Application::resizeWithParent(int cx, int cy)
{
	RECT r;
	int w, h;

	GetClientRect(viewHwnd, &r);

	w = r.right;
	h = r.bottom;

	resize(w, h);
}

RBX::Experimental::Application::Application(HWND wnd)
{
	viewHwnd = wnd;

	GAppSettings _settings;

	_settings.window.resizable = true;
	_settings.window.framed = false;
	_settings.window.stereo = true;
	_settings.window.refreshRate = 32;
	_settings.window.depthBits = 0;
	_settings.dataDir = ConFileInPath("\\content\\");

	window = Win32Window::create(_settings.window, viewHwnd);

	renderDevice = new RenderDevice();
	renderDevice->init(window, 0);

	userInput = new G3D::UserInput();
	window->makeCurrent();

	resizeWithParent();

	sky = Sky::create(renderDevice, _settings.dataDir + "sky/");

	fps = 30.0f;
	applicationThinking = false;

}

SkyRef RBX::getGlobalSky()
{
	return RBXManager::get()->getApplication()->sky;
}
