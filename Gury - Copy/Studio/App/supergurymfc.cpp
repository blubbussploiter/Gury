
// supergurymfc.cpp : Defines the class behaviors for the application.
//

#include <fstream>

#include "../pch.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "../supergurymfc.h"
#include "../MainFrm.h"

#include "../ChildFrm.h"
#include "../CInsertObjectDlg.h"

#include "../supergurymfcDoc.h"
#include "../supergurymfcView.h"

#include "../Documents/htmlDocument.h"
#include "../Documents/scriptEditorDocument.h"
#include "../Documents/scriptEditorView.h"
#include "../Documents/htmlView.h"
#include "../App/GuryChildFrame.h"

#include "../StudioTool.h"

#include "../../Main/framework.h"
#include "../../Gury/Game/Services/crashreporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CsupergurymfcApp

BEGIN_MESSAGE_MAP(CsupergurymfcApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CsupergurymfcApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, &CsupergurymfcApp::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, &CsupergurymfcApp::OnFileNew)
	ON_COMMAND(ID_INSERT_OBJECT, &CsupergurymfcApp::OnInsertObject)
	ON_COMMAND(ID_INSERT_MODEL, &CsupergurymfcApp::OnInsertModel)
	ON_COMMAND(ID_TOOLS_EXECUTESCRIPT, &CsupergurymfcApp::OnExecuteScript)
	ON_COMMAND(ID_TOOLS_LOADCHARACTER, &CsupergurymfcApp::OnLoadCharacter)
	ON_COMMAND(IDR_RUN, &CsupergurymfcApp::OnRunRunService)
	ON_COMMAND(IDR_PAUSE, &CsupergurymfcApp::OnPauseRunService)
	ON_COMMAND(IDR_RESET, &CsupergurymfcApp::OnResetRunService)
	//ON_COMMAND(ID_MOVE_UP, &CsupergurymfcApp::OnMoveUp)
	//ON_COMMAND(ID_MOVE_DOWN, &CsupergurymfcApp::OnMoveDown)
	ON_COMMAND(ID_TILT_LEFT, &CsupergurymfcApp::OnTiltLeft)
	ON_COMMAND(ID_TILT_RIGHT, &CsupergurymfcApp::OnTiltRight)
	ON_COMMAND(ID_TILT_UP, &CsupergurymfcApp::OnTiltUp)
	ON_COMMAND(ID_TILT_DOWN, &CsupergurymfcApp::OnTiltDown)
	ON_COMMAND(ID_ZOOM_IN, &CsupergurymfcApp::OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CsupergurymfcApp::OnZoomOut)
	ON_COMMAND(ID_LOOKAT, &CsupergurymfcApp::OnLookat)
	ON_COMMAND(ID_ZOOM_EXTENTS, &CsupergurymfcApp::OnZoomExtents)
	ON_COMMAND(ID_MOUSE, &CsupergurymfcApp::OnMouseToolSelected)
END_MESSAGE_MAP()


// CsupergurymfcApp construction

CsupergurymfcApp::CsupergurymfcApp() noexcept
{
	m_bHiColorIcons = FALSE;
		
	m_nAppLook = ID_VIEW_APPLOOK_OFF_XP;

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("supergurymfc.AppID.NoVersion"));

	//RBX::Studio::setToolFromType(RBX::Studio::Move);

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CsupergurymfcApp object

CsupergurymfcApp theApp;


// CsupergurymfcApp initialization

BOOL CsupergurymfcApp::InitInstance()
{
	RBX::CrashReporter::get()->start();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_COOL_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	RBX::RBXManager* manager = RBX::RBXManager::get();

	EnableTaskbarInteraction();
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	pGameDocTemplate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(GuryDoc),
		RUNTIME_CLASS(GuryGameFrame),
		RUNTIME_CLASS(GuryView));
	if (!pGameDocTemplate)
		return FALSE;

	AddDocTemplate(pGameDocTemplate);

	pScriptDocTemplate = new CMultiDocTemplate(
		IDR_SCRIPTEDITOR,
		RUNTIME_CLASS(scriptEditDoc),
		RUNTIME_CLASS(GuryChildFrame),
		RUNTIME_CLASS(scriptEditView)
		);

	if (!pScriptDocTemplate)
		return FALSE;

	AddDocTemplate(pScriptDocTemplate);

	startPage = new CMultiDocTemplate(
		IDR_HTMLVIEW,
		RUNTIME_CLASS(htmlPageDoc),
		RUNTIME_CLASS(GuryChildFrame),
		RUNTIME_CLASS(htmlPageView)
	);

	if (!startPage)
		return FALSE;

	AddDocTemplate(startPage);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	CMainFrame::mainFrame = pMainFrame;

	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		MessageBox(0, "Failed to load main gury frame! :(", 0, 0);
		delete pMainFrame;
		return FALSE;
	}

	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!cmdInfo.m_nShellCommand)  // actually none
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	if (!cmdInfo.m_strFileName.IsEmpty())
	{
		manager->toLoad = cmdInfo.m_strFileName.GetString();
	}

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	startPage->OpenDocumentFile(0);

	// The main window has been initialized, so show and update it

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();


	return TRUE;
}

int CsupergurymfcApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	return CWinAppEx::ExitInstance();
}

// CsupergurymfcApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()

void CsupergurymfcApp::OnFileOpen()
{
	static TCHAR BASED_CODE szFilter[] = _T("ROBLOX Place Files (*.rbxl;*.rbxlx)|*.rbxl;*.rbxlx|All Files(*.*)|*.*||");

	CFileDialog dlgFile(TRUE, "Open", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);
	
	if (dlgFile.DoModal() == IDOK)
	{
		RBX::RBXManager* manager = RBX::RBXManager::get();

		CString file = dlgFile.GetPathName();
		CString name = dlgFile.GetFileName();

		manager->fileName = name.GetString();
		manager->toLoad = file.GetString();

		if (pGameDocTemplate)
		{
			pGameDocTemplate->OpenDocumentFile(file);
		}
	}

}

void CsupergurymfcApp::OnFileNew()
{
	RBX::RBXManager::get()->toLoad = std::string();
	if (pGameDocTemplate)
	{
		pGameDocTemplate->OpenDocumentFile(0);
	}
}

// App command to run the dialog
void CsupergurymfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CsupergurymfcApp customization load/save methods

void CsupergurymfcApp::PreLoadState()
{

}

void CsupergurymfcApp::LoadCustomState()
{
}

void CsupergurymfcApp::SaveCustomState()
{
}

// CsupergurymfcApp message handlers


//void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
//{
	// TODO: Add your message handler code here and/or call default

//	CDialogEx::OnTimer(nIDEvent);
//}
