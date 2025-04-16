#include "htmlView.h"

IMPLEMENT_DYNCREATE(htmlPageView, CView)
BEGIN_MESSAGE_MAP(htmlPageView, CView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

htmlPageView::htmlPageView() noexcept
{
}

void htmlPageView::OnDraw(CDC* pDC)
{
	htmlPageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: add draw code for native data here
}

BOOL htmlPageView::PreCreateWindow(CREATESTRUCT& cs)
{
	
	return 0;
}

htmlPageView::~htmlPageView()
{
}

int htmlPageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndHtmlView.Create(0, 0, dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Output View\n");
		return -1;      // fail to create
	}

	m_wndHtmlView.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	return 0;
}
