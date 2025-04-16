#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Main/Resource.h"

class GuryChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(GuryChildFrame)
public:
	GuryChildFrame() noexcept;

	// Attributes

protected:
	CSplitterWnd m_wndSplitter;
	// Operations
public:

	// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// Implementation
public:
	virtual ~GuryChildFrame();
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};