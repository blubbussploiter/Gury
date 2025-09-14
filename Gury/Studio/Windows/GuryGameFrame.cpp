
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "../Gury/Game/Network/Player/Mouse.h"   
#include "../../Main/Resource.h"

#include "../../Main/framework.h"
#include "../supergurymfc.h"

#include "../ChildFrm.h"
#include "../MainFrm.h"

void suspendCurrentApplication(RBX::Experimental::Application* application)
{
	RBX::RBXManager* manager = RBX::RBXManager::get();

	if (manager->getApplication())
	{
		manager->getApplication()->suspend();
	}

	manager->setCurrentApplication(application);
	manager->getApplication()->resume();
}

void createApplication(GuryGameFrame* frame)
{
	RBX::RBXManager* manager = RBX::RBXManager::get();

	if (!frame->application)
	{
		RBX::Experimental::Application* app = manager->instantiate(frame->GetSafeHwnd());

		if (!manager->toLoad.empty())
		{
			app->rbxlFile = manager->toLoad;
		}

		frame->application = app;
		suspendCurrentApplication(frame->application);

		frame->application->start();
	}
}

// CChildFrame

IMPLEMENT_DYNCREATE(GuryGameFrame, CMDIChildWndEx)
BEGIN_MESSAGE_MAP(GuryGameFrame, CMDIChildWndEx)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
//	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_MOUSEWHEEL()
//	ON_WM_SIZING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	//	ON_WM_PAINT()
	//	ON_WM_CREATE()
	//	ON_WM_TIMER()
	//	ON_WM_PAINT()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
ON_WM_KEYDOWN()
ON_WM_SYSKEYDOWN()
ON_WM_DESTROY()
END_MESSAGE_MAP()

// CChildFrame construction/destruction

GuryGameFrame::GuryGameFrame() noexcept
{
	// TODO: add member initialization code here
	application = 0;
}

GuryGameFrame::~GuryGameFrame()
{

}

BOOL GuryGameFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	cs.lpszClass = AfxRegisterWndClass(
		CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, // use any window styles
		RBX::Mouse::get()->get_cursor(),
		(HBRUSH)(COLOR_WINDOW + 1)); // background brush
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

void GuryGameFrame::OnSetFocus(CWnd* pNewWnd)
{
	CMDIChildWnd::OnSetFocus(pNewWnd);
}

BOOL GuryGameFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{

	createApplication(this);
	SetTimer(1, (UINT)(application->fps-5), NULL);

	return 1;
}

void GuryGameFrame::OnKillFocus(CWnd* pNewWnd)
{
	CMDIChildWnd::OnKillFocus(pNewWnd);
}

BOOL GuryGameFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (application)
	{
		application->getCamera()->cam_zoom(zDelta > 0);
	}

	return CMDIChildWndEx::OnMouseWheel(nFlags, zDelta, pt);
}

//void CChildFrame::OnSize(UINT nType, int cx, int cy)
//{
	// TODO: Add your message handler code here
//	CMDIChildWndEx::OnSize(nType, cx, cy);
//}

//void CChildFrame::OnSizing(UINT fwSide, LPRECT pRect)
//{
//
//	CMDIChildWndEx::OnSizing(fwSide, pRect);
//
	// TODO: Add your message handler code here
//}

void GuryGameFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{

	CMDIChildWndEx::OnWindowPosChanged(lpwndpos);

	if (application)
	{
		if (application->renderDevice) {

			int cx = lpwndpos->cx;
			int cy = lpwndpos->cy;

			Rect2D viewport = application->renderDevice->getViewport();
			application->renderDevice->notifyResize(cx, cy);
			application->renderDevice->setViewport(Rect2D::xywh(0, 0, application->renderDevice->getWidth(), application->renderDevice->getHeight()));

			application->onGraphics();
		}
	}

	// TODO: Add your message handler code here
}


void GuryGameFrame::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{

	CMDIChildWndEx::OnWindowPosChanging(lpwndpos);

	if (application)
	{
		if (application->renderDevice) {

			int cx = lpwndpos->cx;
			int cy = lpwndpos->cy;

			Rect2D viewport = application->renderDevice->getViewport();
			application->renderDevice->notifyResize(cx, cy);
			application->renderDevice->setViewport(Rect2D::xywh(0 ,0, application->renderDevice->getWidth(), application->renderDevice->getHeight()));

			application->onGraphics();
		}
	}

	// TODO: Add your message handler code here
}

void GuryGameFrame::OnPaint() /* Old step handler - just in case */
{
	CPaintDC dc(this);
	CMDIChildWndEx::OnPaint();
}

void GuryGameFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (application)
		{
			application->mainProcessStep();
		}
	}
	CMDIChildWndEx::OnTimer(nIDEvent);
}


int GuryGameFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

bool GuryGameFrame::doFinalizePlaceChanges()
{
	if (application)
	{
		const int result = MessageBoxA(RBX::Format("Save changes to %s?", application->appName.c_str()).c_str(), "Gury", MB_YESNOCANCEL | MB_ICONQUESTION);

		switch (result)
		{
		case IDYES:
		{
			CMainFrame::mainFrame->OnFileSave();
			return 1;
		}
		case IDNO:
		{
			return 1;
		}
		default: break;
		}
	}
	return 0;
}

void GuryGameFrame::OnClose()
{
	RBX::RBXManager* manager = RBX::RBXManager::get();

	if (doFinalizePlaceChanges())
	{

		KillTimer(1);

		if (application == manager->getApplication())
		{
			closeEventCalled = true;

			manager->closeCurrentApplication();
			this->application->isThinking = false;
			this->application = nullptr;
		}

		CMDIChildWndEx::OnClose();
	}

}

void GuryGameFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	setGameViewFocus(point);
	CMDIChildWndEx::OnLButtonDown(nFlags, point);
}


void GuryGameFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	setGameViewFocus(point);
	CMDIChildWndEx::OnRButtonDown(nFlags, point);
}

void GuryGameFrame::setGameViewFocus(POINT c)
{
	/* Focus back into game view */

	RECT rect;
	GetWindowRect(&rect);

	if (c.x > rect.left && c.x < rect.right && c.y > rect.top && c.y < rect.bottom)
	{
		SetActiveWindow();
		SetFocus();
	}
}

void GuryGameFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{

	if (application)
	{
		SetWindowText(application->appName.c_str());
	}

	CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);
}

void GuryGameFrame::OnDestroy()
{
	CMDIChildWndEx::OnDestroy();

	KillTimer(1);
}
