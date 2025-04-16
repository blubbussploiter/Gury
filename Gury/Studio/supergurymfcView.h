
// supergurymfcView.h : interface of the CsupergurymfcView class
//

#pragma once

#include "supergurymfcDoc.h"
#include "../Gury/Application/appmanager.h"

class GuryView : public CView
{
protected: // create from serialization only
	GuryView() noexcept;
	DECLARE_DYNCREATE(GuryView)

// Attributes
public:

	GuryDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
//	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~GuryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // debug version in supergurymfcView.cpp
inline GuryDoc* GuryView::GetDocument() const
   { return reinterpret_cast<GuryDoc*>(m_pDocument); }
#endif

