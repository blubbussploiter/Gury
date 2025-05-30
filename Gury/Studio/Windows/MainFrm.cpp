
// MainFrm.cpp : implementation of the CMainFrame class
//


#include "../pch.h"
#include "../../Main/framework.h"
#include "../supergurymfc.h"

#include "../Gury/Application/appmanager.h"
#include "../Gury/Game/World/runservice.h"
#include "../Gury/Game/Serializer/serializer.h"

#include "../ChildFrm.h"
#include "../MainFrm.h"

#include "../StudioTool.h"
#include "../robloxClipboard.h"
#include "../supergurymfcView.h"
#include "../App/GuryChildFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GURY_WNDTEXT "Gury3d - development branch"
CMainFrame* CMainFrame::mainFrame = 0;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_SETTINGCHANGE()
	ON_WM_CREATE()	
	ON_WM_SIZE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &CMainFrame::OnTabChange)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_VS_2005, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_UPDATE_COMMAND_UI(AFX_IDP_SQL_NO_POSITIONED_UPDATES, &CMainFrame::OnUpdateCommandId)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateClipboardContent)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_TILT, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_ROTATE, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_MOVE_UP, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_MOVE_DOWN, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(IDR_RUN, &CMainFrame::OnUpdateRunServiceIsntRunningButton)
	ON_UPDATE_COMMAND_UI(IDR_PAUSE, &CMainFrame::OnUpdateRunServiceIsRunningButton)
	ON_UPDATE_COMMAND_UI(IDR_RESET, &CMainFrame::OnUpdateRunServiceIsRunningOrPausedButton)
	ON_UPDATE_COMMAND_UI(ID_MOUSE, &CMainFrame::OnUpdateStudioToolsButton)
	ON_UPDATE_COMMAND_UI(ID_MOVE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_RESIZE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_LOCK, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_ANCHOR, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_COLOR, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_SAMPLE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_SMOOTH, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_GLUE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_WELD, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_STUD, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_INLET, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_HINGE, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_MOTOR, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_TILT_LEFT, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_TILT_RIGHT, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_TILT_UP, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_TILT_DOWN, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CMainFrame::OnUpdateToolbarButton)
	ON_UPDATE_COMMAND_UI(ID_LOOKAT, &CMainFrame::OnUpdateSelectionButton)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_EXTENTS, &CMainFrame::OnUpdateToolbarButton)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CMainFrame::OnEditCut)
//	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPasteinto)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_SAVE, &CMainFrame::OnFileSave)
//	ON_WM_WINDOWPOSCHANGING()
//	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
	m_bAutoMenuEnable = 0;
}

CMainFrame::~CMainFrame()
{
}

//BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
//{
//	/* Create start page */
//
//	m_startPage = (CStartPage*)CreateObject();
//	if (!m_startPage->Create("RBXHtml", "Start Page", WS_CHILD | WS_VISIBLE, rectDefault, this)) {
//		TRACE0("Failed to create start Page\n");
//		return -1;      // fail to create
//	}
//
//	return CMDIFrameWndEx::OnCreateClient(lpcs, pContext);
//}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_VS2005; // other styles available...
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = FALSE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area=
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndMainTools.CreateEx(this, TBSTYLE_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndMainTools.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndRunServiceTools.CreateEx(this, TBSTYLE_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndRunServiceTools.LoadToolBar(IDR_RUNSERVICE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndCameraTools.CreateEx(this, TBSTYLE_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndCameraTools.LoadToolBar(CAMERA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStudioTools.CreateEx(this, TBSTYLE_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndStudioTools.LoadToolBar(SERVICES))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMainTools.EnableDocking(CBRS_ALIGN_ANY);
	m_wndRunServiceTools.EnableDocking(CBRS_ALIGN_ANY);
	m_wndStudioTools.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCameraTools.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndMainTools);
	m_wndMainTools.ShowPane(1, 0, 1);
	DockPaneLeftOf(&m_wndRunServiceTools, &m_wndMainTools);
	m_wndRunServiceTools.ShowPane(1, 0, 1);
	DockPaneLeftOf(&m_wndStudioTools, &m_wndRunServiceTools);
	m_wndStudioTools.ShowPane(1, 0, 1);
	DockPaneLeftOf(&m_wndCameraTools, &m_wndStudioTools);
	m_wndCameraTools.ShowPane(1, 0, 1);

	m_wndMainTools.SetWindowTextA(_T("Standard"));
	m_wndRunServiceTools.SetIgnoreSetText(false);
	m_wndRunServiceTools.SetWindowTextA(_T("Test"));
	m_wndCameraTools.SetWindowTextA(_T("Camera"));
	m_wndStudioTools.SetWindowTextA(_T("Mouse"));

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndClassView);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, "Customize", ID_VIEW_TOOLBAR);

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(IDR_RUN);
	lstBasicCommands.AddTail(IDR_PAUSE);
	lstBasicCommands.AddTail(IDR_RESET);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	SetWindowText(GURY_WNDTEXT);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_GAME_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CFrameWnd* frame = GetActiveFrame();

	if (frame) {

		GuryGameFrame* pChild = dynamic_cast<GuryGameFrame*>(frame);

		std::string text;

		if (pChild) {
			if (pChild->application && (int)pChild->application != -1)
			{
				if (bAddToTitle)
				{
					text = pChild->application->appName;
				}
			}
		}
		else
		{
			CDocument* document = GetActiveDocument();
			if (document)
			{
				text = document->GetTitle();
			}
		}

		std::string concat = RBX::Format("%s - [%s]", GURY_WNDTEXT, text.c_str());
		SetWindowText(concat.c_str());
	}
	else
	{
		SetWindowText(GURY_WNDTEXT);
	}
}

LRESULT CMainFrame::OnTabChange(WPARAM wp, LPARAM lp)
{
	GuryGameFrame* gameFrame = dynamic_cast<GuryGameFrame*>(GetActiveFrame());
	GuryChildFrame* childFrame = dynamic_cast<GuryChildFrame*>(GetActiveFrame());
	RBX::RBXManager* manager = RBX::RBXManager::get();
	RBX::Experimental::Application* current = manager->getApplication();

	if (wp != -1)
	{
		if (gameFrame)
		{
			if (gameFrame->application)
			{
				if (current != gameFrame->application)
				{
					manager->setCurrentApplication(gameFrame->application);
					gameFrame->application->getDatamodel()->emptyExplorerWindow();
					gameFrame->application->getDatamodel()->fillExplorerWindow();
				}
				else
				{
					gameFrame->application->resume();
				}
			}
		}
		else
		{
			if (childFrame)
			{
				if (current)
				{
					current->suspend();
				}
			}
		}
	}

	return 0;
}

//void CMainFrame::OnApplicationLook(UINT id)
//{
//	CWaitCursor wait;
//	theApp.m_nAppLook = id;
//
//	switch (theApp.m_nAppLook)
//	{
//	case ID_VIEW_APPLOOK_WIN_2000:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
//		break;
//
//	case ID_VIEW_APPLOOK_OFF_XP:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
//		break;
//
//	case ID_VIEW_APPLOOK_WIN_XP:
//		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
//		break;
//
//	case ID_VIEW_APPLOOK_OFF_2003:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
//		CDockingManager::SetDockingMode(DT_SMART);
//		break;
//
//	case ID_VIEW_APPLOOK_VS_2005:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
//		CDockingManager::SetDockingMode(DT_SMART);
//		break;
//
//	case ID_VIEW_APPLOOK_VS_2008:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
//		CDockingManager::SetDockingMode(DT_SMART);
//		break;
//
//	case ID_VIEW_APPLOOK_WINDOWS_7:
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
//		CDockingManager::SetDockingMode(DT_SMART);
//		break;
//
//	default:
//		switch (theApp.m_nAppLook)
//		{
//		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
//			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
//			break;
//
//		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
//			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
//			break;
//
//		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
//			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
//			break;
//
//		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
//			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
//			break;
//		}
//
//		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
//		CDockingManager::SetDockingMode(DT_SMART);
//	}
//
//	m_wndOutput.UpdateFonts();
//	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
//
//	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
//}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


//BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
//{
	// base class does the real work

//	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

void CMainFrame::OnUpdateCommandId(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here

}


void CMainFrame::OnUpdateToolbarButton(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(RBX::RBXManager::get()->getApplication() != 0);
}

void CMainFrame::OnUpdateSelectionButton(CCmdUI* pCmdUI)
{
	if (!RBX::Datamodel::get()) {
		pCmdUI->Enable(0);
		return;
	}
	pCmdUI->Enable(RBX::SelectionService::get()->selection.size() > 0);
}

void CMainFrame::OnUpdateRunServiceIsntRunningButton(CCmdUI* pCmdUI)
{
	if (!RBX::Datamodel::get()) {
		pCmdUI->Enable(0);
		return;
	}
	RBX::RunService* runService = RBX::RunService::get();
	if(runService)
	{
		pCmdUI->Enable(!runService->isRunning);
	}
}

void CMainFrame::OnUpdateClipboardContent(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(RBX::StudioClipboard::get()->isEmpty());
}

void CMainFrame::OnUpdateRunServiceIsRunningOrPausedButton(CCmdUI* pCmdUI)
{
	if (!RBX::Datamodel::get()) {
		pCmdUI->Enable(0);
		return;
	}
	RBX::RunService* runService = RBX::RunService::get();
	if (runService)
	{
		pCmdUI->Enable(runService->isRunning || runService->isPaused);
	}
}

void CMainFrame::OnUpdateRunServiceIsRunningButton(CCmdUI* pCmdUI)
{
	if (!RBX::Datamodel::get()) {
		pCmdUI->Enable(0);
		return;
	}
	RBX::RunService* runService = RBX::RunService::get();
	if (runService)
	{
		pCmdUI->Enable(runService->isRunning);
	}
}

void CMainFrame::OnUpdateStudioToolsButton(CCmdUI* pCmdUI)
{
	bool e;
	RBX::Datamodel* dm;

	dm = RBX::Datamodel::get();
	switch (pCmdUI->m_nID)
	{
		case ID_MOUSE:
		{
			e = RBX::Studio::currentToolType(RBX::Studio::Mouse);
			break;
		}
		case ID_MOVE:
		{
			e = RBX::Studio::currentToolType(RBX::Studio::Move);
			break;
		}
	}
	e = (e && dm != 0);
	pCmdUI->SetRadio(e);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);
}


BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CMDIFrameWndEx::OnSetCursor(pWnd, nHitTest, message);
}


void CMainFrame::OnEditCopy()
{

	RBX::Instances which = RBX::SelectionService::get()->selectionAsInstances();
	if (which.size() > 0)
	{
		RBX::StudioClipboard::get()->copyMultiple(&which);
	}
}


void CMainFrame::OnEditCut()
{
	RBX::Instances which = RBX::SelectionService::get()->selectionAsInstances();
	if (which.size() > 0)
	{
		RBX::StudioClipboard::get()->cutMultiple(&which);
	}
}


void CMainFrame::OnEditPasteinto()
{
	RBX::Instances which = RBX::SelectionService::get()->selectionAsInstances();
	if (which.size() > 0)
	{
		RBX::Instance* fromSelectable = *which.begin();
		if (fromSelectable) {
			RBX::StudioClipboard::get()->paste(fromSelectable);
		}
	}
	else
	{
		RBX::StudioClipboard::get()->paste(RBX::Workspace::get());
	}
}


void CMainFrame::OnDestroy()
{
	/* probably have save thing or something, check history service */

	CMDIFrameWndEx::OnDestroy();

}


void CMainFrame::OnFileSave()
{

	RBX::RBXManager* manager = RBX::RBXManager::get();
	RBX::Experimental::Application* application = manager->getApplication();

	if (application)
	{
		if (application->rbxlFile.empty())
		{

			static TCHAR BASED_CODE szFilter[] = _T("ROBLOX Place Files (*.rbxl;*.rbxlx)|*.rbxl;*.rbxlx|All Files(*.*)|*.*||");

			CFileDialog dlgFile(FALSE, "Save", 0, OFN_OVERWRITEPROMPT, szFilter);

			if (dlgFile.DoModal() == IDOK)
			{
				CString file = dlgFile.GetPathName();

				application->rbxlFile = file.GetString();
				application->updateAppName();

				GetActiveFrame()->OnUpdateFrameTitle(1);
			}

		}
		RBX::Datamodel::get()->saveContent(application->rbxlFile);
	}

}

//void CMainFrame::OnWindowPosChanging(WINDOWPOS* lpwndpos)
//{
//	CFrameWnd* wnd = GetActiveFrame();
//	CChildFrame* child = (CChildFrame*)(wnd);
//
//	if (child && (int)child->application != -1)
//	{
//		if (!child->application->inEditMode) {
//			child->OnWindowPosChanging(lpwndpos);
//		}
//	}
//
//	CMDIFrameWndEx::OnWindowPosChanging(lpwndpos);
//
//}

//void CMainFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
//{
//	CFrameWnd* wnd = GetActiveFrame();
//	CChildFrame* child = (CChildFrame*)(wnd);
//
//	if (child && (int)child->application != -1)
//	{
//		if (!child->application->inEditMode) {
//			child->OnWindowPosChanged(lpwndpos);
//		}
//	}
//
//	CMDIFrameWndEx::OnWindowPosChanged(lpwndpos);
//
//}
