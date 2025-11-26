
// supergurymfcView.cpp : implementation of the CsupergurymfcView class
//

#include <thread>

#include "../pch.h"
#include "../../Main/framework.h"

#ifndef SHARED_HANDLERS
#include "../supergurymfc.h"
#endif

#include "../supergurymfcDoc.h"
#include "../supergurymfcView.h"

#include "../MainFrm.h"

// CsupergurymfcView

IMPLEMENT_DYNCREATE(GuryView, CView)

BEGIN_MESSAGE_MAP(GuryView, CView)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
	ON_WM_MOUSEWHEEL()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CsupergurymfcView construction/destruction

GuryView::GuryView() noexcept
{
	// TODO: add construction code here
}

GuryView::~GuryView()
{
}

void GuryView::OnDraw(CDC* /*pDC*/)
{
	// TODO: Add your specialized code here and/or call the base class
}

BOOL GuryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.lpszClass = AfxRegisterWndClass(
		CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, // use any window styles
		RBX::Mouse::get()->get_cursor(),
		(HBRUSH)(COLOR_WINDOW + 1)); // background brush

	return CView::PreCreateWindow(cs);
}

// CsupergurymfcView drawing

void GuryView::OnClose()
{
	CView::OnClose();
}

int GuryView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void GuryView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
}

void GuryView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}

BOOL GuryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void GuryView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);
}

void GuryView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
}


void GuryView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CView::OnPaint() for painting messages
}

void GuryView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}

void GuryView::OnTimer(UINT_PTR nIDEvent)
{
	CView::OnTimer(nIDEvent);
}

void GuryView::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CView::OnWindowPosChanged(lpwndpos);

}

void GuryView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CView::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}
