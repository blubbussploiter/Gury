#include "GuryChildFrame.h"

IMPLEMENT_DYNCREATE(GuryChildFrame, CMDIChildWndEx)
BEGIN_MESSAGE_MAP(GuryChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

GuryChildFrame::GuryChildFrame() noexcept
{
}

BOOL GuryChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

BOOL GuryChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (!CMDIChildWndEx::OnCreateClient(lpcs, pContext))
		return FALSE;

	return TRUE;
}

GuryChildFrame::~GuryChildFrame()
{
}
