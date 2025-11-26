
#include "../pch.h"
#include "../MainFrm.h"
#include "../ClassView.h"
#include "../supergurymfc.h"

#include "../../Main/Resource.h"
#include "../../Main/framework.h"

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/stdout.h"

std::map<std::string, int> instanceImages =
{
	{"Instance", 0},
	{"Part", 1},
	{"PVInstance", 1},
	{"Script", 3},
	{"BaseScript", 3},
	{"Decal", 4},
	{"SpecialMesh", 5},
	{"Humanoid", 6},
	{"SoundService", 8},
	{"Players", 9},
	{"Player", 9},
	{"Lighting", 10},
	{"Script", 11},
	{"Workspace", 12},
	{"Model", 12},
	{"Debris", 13},
	{"NetworkClient", 14},
	{"Hint", 14},
	{"Message", 14}
};

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class ExplorerTreeView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExplorerTreeView::ExplorerTreeView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

ExplorerTreeView::~ExplorerTreeView()
{
}

BEGIN_MESSAGE_MAP(ExplorerTreeView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateClipboardContent)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int ExplorerTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	
	OnChangeVisualStyle();

	CMenu menuSort;
	menuSort.LoadMenu(IDR_OUTPUT_POPUP);

	return 0;
}

void ExplorerTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void ExplorerTreeView::AddInstance(RBX::Instance* instance, bool addDescending)
{
	RBX::Instance* parent = instance->getParent();
	if (instance && parent) {

		std::string name = instance->getName();
		HTREEITEM parentItem = GetInstance(parent);

		if (parentItem
			|| parent == RBX::Datamodel::get())
		{
			int i = instanceImages[instance->getClassName()];

			HTREEITEM instanceItem = m_wndClassView.InsertItem(name.c_str(), i, i, parentItem);
			items.set(instance, instanceItem);

			if (addDescending)
			{
				RBX::Instances* instances = instance->getChildren();
				for (unsigned int i = 0; i < instances->size(); i++)
				{
					AddInstance(instances->at(i));
				}
			}
		}

	}
}

void ExplorerTreeView::RemoveInstance(RBX::Instance* instance)
{
	HTREEITEM item = GetInstance(instance);

	if (item)
	{
		m_wndClassView.DeleteItem(item);
		m_wndClassView.RedrawWindow();

		items.remove(instance);
	}
}

void ExplorerTreeView::SelectInstance(RBX::Instance* i)
{
	m_wndClassView.SelectInstance(i);
}

void ExplorerTreeView::ClearInstances()
{
	Array<RBX::Instance*> keys;

	keys = items.getKeys();

	for (int i = 0; i < keys.size(); i++)
	{
		m_wndClassView.DeleteItem(items[keys[i]]);
		if (items.containsKey(keys[i]))
		{
			items.remove(keys[i]);
		}
	}

	m_wndClassView.RedrawWindow();
}

HTREEITEM ExplorerTreeView::GetInstance(RBX::Instance* instance)
{
	if (items.containsKey(instance))
	{
		return items.get(instance);
	}
	return 0;
}

RBX::Instance* ExplorerTreeView::GetTreeItem(HTREEITEM item)
{
	Array<RBX::Instance*> keys;

	keys = items.getKeys();

	for (int i = 0; i < keys.size(); i++)
	{
		if (items.get(keys[i]) == item)
		{
			return keys[i];
		}
	}

	return 0;
}

void ExplorerTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	SetFocus();

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void ExplorerTreeView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top, rectClient.Width() - 2, rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL ExplorerTreeView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void ExplorerTreeView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;
}

void ExplorerTreeView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void ExplorerTreeView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void ExplorerTreeView::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void ExplorerTreeView::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void ExplorerTreeView::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void ExplorerTreeView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}

void ExplorerTreeView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
}

void ExplorerTreeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void ExplorerTreeView::OnChangeVisualStyle()
{
	m_wndClassView.m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_wndClassView.m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_wndClassView.m_ClassViewImages.Add(&bmp, RGB(255, 0, 220));

	m_wndClassView.SetImageList(&m_wndClassView.m_ClassViewImages, TVSIL_NORMAL);
}

void ExplorerTreeView::MoveInstanceParent(RBX::Instance* instance)
{

}
