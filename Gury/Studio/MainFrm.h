
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "pch.h"

#include "ClassView.h"
#include "CCommandBar.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

#include "Windows/startPage.h"

#include "../Main/framework.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar              m_wndMainTools;
	CMFCToolBar              m_wndRunServiceTools;
	CMFCToolBar              m_wndStudioTools;
	CMFCToolBar              m_wndCameraTools;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	ExplorerTreeView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CStartPage* m_startPage;

	static CMainFrame* mainFrame;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnTabChange(WPARAM wp, LPARAM lp);
//	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void onFullScreen();
	void OnUpdateFrameTitle(BOOL bAddToTitle);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnUpdateCommandId(CCmdUI* pCmdUI);

	afx_msg void OnUpdateToolbarButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectionButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunServiceIsntRunningButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunServiceIsRunningOrPausedButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunServiceIsRunningButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStudioToolsButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClipboardContent(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
//	afx_msg void OnEditPaste();
	afx_msg void OnEditPasteinto();
	afx_msg void OnDestroy();
	afx_msg void OnFileSave();
//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
//	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


