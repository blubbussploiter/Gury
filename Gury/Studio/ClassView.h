
#pragma once

#include <map>
#include <string>

#include "G3D/Table.h"
#include "ViewTree.h"

namespace RBX
{
	class Instance;
}

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class ExplorerTreeView : public CDockablePane
{
public:
	G3D::Table<RBX::Instance*, HTREEITEM> items;

	ExplorerTreeView() noexcept;
	virtual ~ExplorerTreeView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	void MoveInstanceParent(RBX::Instance* instance);

	void AddInstance(RBX::Instance* instance, bool addDescending=true);
	void RemoveInstance(RBX::Instance* instance);

	void SelectInstance(RBX::Instance* i);
	void ClearInstances();

	HTREEITEM GetInstance(RBX::Instance* instance);
	RBX::Instance* GetTreeItem(HTREEITEM item);

public:

	CViewTree m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

