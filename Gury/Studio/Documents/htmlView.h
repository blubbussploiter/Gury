#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Main/Resource.h"

#include "../Windows/rbxhtml.h"

#include "htmlDocument.h"

class htmlPageView : public CView
{
protected: // create from serialization only
	DECLARE_DYNCREATE(htmlPageView)
	htmlPageView() noexcept;

	CRBXHtmlView m_wndHtmlView;
	// Attributes
public:

	htmlPageDoc* GetDocument() const;

	// Operations
public:

	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	// Implementation
public:
	virtual ~htmlPageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in supergurymfcView.cpp
inline htmlPageDoc* htmlPageView::GetDocument() const
{
	return reinterpret_cast<htmlPageDoc*>(m_pDocument);
}
#endif

