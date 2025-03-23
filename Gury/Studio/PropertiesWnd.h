
#pragma once

#include <map>

#include "../Gury/Main/framework.h"
#include "../Gury/Game/Objects/instance.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class RobloxProperty : public CMFCPropertyGridProperty
{
public:

	RBX::Instance* object;

	RobloxProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = 0) : CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
	{

	}

	RobloxProperty(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,	LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,LPCTSTR lpszValidChars = NULL) : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
	{

	}

	BOOL OnEndEdit();
};

class CPropertiesWnd : public CDockablePane
{
private:
	std::map<RBX::Types,CMFCPropertyGridProperty*> catergories;
// Construction
public:

	CPropertiesWnd() noexcept;

	void AdjustLayout();

	void ClearProperties();
	void FillProperties(RBX::Instance* i);

	void AddProperty(RBX::Instance* i, rttr::property p);

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:

	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;

// Implementation
public:
	CMFCPropertyGridCtrl m_wndPropList;
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();

};

