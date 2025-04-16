#include "startPage.h"

IMPLEMENT_DYNCREATE(CStartPage, CMDIChildWndEx)
BEGIN_MESSAGE_MAP(CStartPage, CMDIChildWndEx)
END_MESSAGE_MAP()

CStartPage::CStartPage() noexcept
{
}

CStartPage::~CStartPage()
{
}

BOOL CStartPage::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIChildWndEx::PreCreateWindow(cs);
}

BOOL CStartPage::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
}
