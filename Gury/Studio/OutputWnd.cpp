

#include "OutputWnd.h"
#include "MainFrm.h"

#include "../Main/Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

#define style "font-family:\"Tahoma\", sans-serif; font-size: calc(var(--total-scale-factor) *4.00px); transform: scale(1.5, 0.75);"

int r = 0, g = 0, b = 0;

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

COutputView::COutputView() noexcept
{

}

COutputView::~COutputView()
{
	Stop();
	OnQuit();
}

void COutputView::WriteText(std::string preText, std::string text, std::string color)
{

	IHTMLDocument2* pDoc = 0;
	LPDISPATCH pDisp = GetHtmlDocument();

	if (pDisp && SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)))
	{
		IHTMLElement* pBody;
		pDoc->get_body(&pBody);

		if (pBody)
		{
			std::string bold = std::string();
			if (color != "blue")
			{
				bold = "font-weight:bold;";
			}
			else { bold = "font-weight:normal;"; }

			std::string html = RBX::Format("<span style='%s'>%s</span><span style='%s color:%s; %s;'>%s</span><br>", style, preText.c_str(), style, color.c_str(), bold.c_str(), text.c_str());

			BSTR bHtml = _bstr_t(html.c_str());
			pBody->insertAdjacentHTML(_bstr_t("beforeEnd"), bHtml);
			pBody->Release();
		}

		pDisp->Release();
	}

}

void COutputView::WriteQuickText(std::string text)
{
	IHTMLDocument2* pDoc = 0;
	LPDISPATCH pDisp = GetHtmlDocument();

	if (pDisp && SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)))
	{
		IHTMLElement* pBody;
		pDoc->get_body(&pBody);

		if (pBody)
		{
			std::string html = RBX::Format("<span style='%s'>%s</span><br>", style, text.c_str());

			BSTR bHtml = _bstr_t(html.c_str());
			pBody->insertAdjacentHTML(_bstr_t("beforeEnd"), bHtml);
			pBody->Release();
		}

		pDisp->Release();
	}

}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(COutputView, CHtmlView)
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
	r = r0;
	g = g0;
	b = b0;
}

void COutputView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// Load a blank document
	Navigate2(_T("about:blank"), NULL, NULL);

}