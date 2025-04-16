
// ChildFrm.h : interface of the CChildFrame class
//

#pragma once

#include "../Gury/Application/appmanager.h"

/* WARNING: DO NOT USE THIS AS A CCHILDFRAME! 

	// I wrote this when i was NEW to MFC, and i completely messed up CChildframe, please use GuryChildFrame for any additional addons for mfc!
*/

class GuryGameFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(GuryGameFrame)
public:
	GuryGameFrame() noexcept;

	// Attributes

protected:
	CSplitterWnd m_wndSplitter;

public:

	bool firstPaintCalled;
	bool closeEventCalled;
	HCURSOR guryCursor;

	RBX::Experimental::Application* application;

	// Operations
public:

	// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	bool doFinalizePlaceChanges();

	afx_msg void OnSetFocus(CWnd* pNewWnd);

	// Implementation
public:
	virtual ~GuryGameFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
//	afx_msg void OnPaint();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnPaint();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void setGameViewFocus(POINT c);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	afx_msg void OnDestroy();
};

