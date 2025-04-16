#pragma once

#include "rbxhtml.h"

class CStartPage : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CStartPage)
private:
	// Construction
	//CRBXHtmlView htmlView;
public:
	CStartPage() noexcept;

public:
	virtual ~CStartPage();

protected:

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};