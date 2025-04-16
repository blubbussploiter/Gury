

#include "../OutputWnd.h"
#include "../MainFrm.h"

#include "../../Main/Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutput.Create(0, 0, dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Output View\n");
		return -1;      // fail to create
	}

	m_wndOutput.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_wndOutput && IsWindow(m_wndOutput.GetSafeHwnd()))
	{
		CRect rectClient;
		GetClientRect(rectClient);
		m_wndOutput.MoveWindow(rectClient);
	}
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax =
			(((cxExtentMax) > ((int)dc.GetTextExtent(strItem).cx)) ? (cxExtentMax) : ((int)dc.GetTextExtent(strItem).cx));
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::UpdateFonts()
{
	SetFont(&afxGlobalData.fontRegular);
}

void COutputWnd::SetTextColor(int r0, int g0, int b0)
{
}

void COutputWnd::AddText(RBX::MessageType messageType, std::string preText, std::string Text)
{
	switch (messageType)
	{
	case RBX::MESSAGE_INFO:
	{
		m_wndOutput.WriteText(preText, Text, "blue");
		break;
	}
	case RBX::MESSAGE_ERROR:
	{
		m_wndOutput.WriteText(preText, Text, "red");
		break;
	}
	case RBX::MESSAGE_WARNING:
	{
		m_wndOutput.WriteText(preText, Text, "orange");
		break;
	}
	}
}
