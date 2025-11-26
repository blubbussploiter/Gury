
#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Game/Services/selection.h"

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

class CViewTree : public CTreeCtrl
{
private:
	RBX::Instance* lastSelected;
	std::vector<RBX::Instance*> selection;

	bool m_isDraggingItem;
	HTREEITEM hDraggedItem;
// Construction
public:

	CImageList m_ClassViewImages;

	CViewTree() noexcept;

	void DeselectAll();

	void DeselectItem(HTREEITEM item);
	void SelectWorldItem(HTREEITEM item, BOOL multiSelect);

	void ExpandAndFocusOnItem(RBX::Instance* i, HTREEITEM item);
	void SelectInstance(RBX::Instance* instance);

	void TryDoubleClick(HTREEITEM item);

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
