
#include "pch.h"
#include "../Gury/Main/framework.h"
#include "ViewTree.h"
#include "MainFrm.h"

#include "../Gury/Game/Services/stdout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree() noexcept
{
}

void CViewTree::DeselectAll()
{
    for (unsigned int i = 0; i < selection.size(); i++)
    {
        RBX::Instance* s = selection.at(i);
        HTREEITEM item = CMainFrame::mainFrame->m_wndClassView.GetInstance(s);
        DeselectItem(item);
    }
    selection.clear();
}

void CViewTree::DeselectItem(HTREEITEM item)
{
    RBX::Instance* i = CMainFrame::mainFrame->m_wndClassView.GetTreeItem(item);

    if (i)
    {
        RBX::SelectionService::get()->deselect(i);

        SetItemState(item, 0, LVIS_SELECTED);

        if (std::find(selection.begin(), selection.end(), i) != selection.end())
        {
            selection.erase(std::remove(selection.begin(), selection.end(), i));
        }

        CMainFrame::mainFrame->m_wndProperties.ClearProperties();
    }
}

void CViewTree::SelectWorldItem(HTREEITEM item, BOOL multiSelect)
{
    RBX::Instance* i = CMainFrame::mainFrame->m_wndClassView.GetTreeItem(item);

    if (i)
    {
        SetItemState(item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

        lastSelected = i;

        if (!multiSelect)
        {
            if (lastSelected)
            {
                RBX::SelectionService::get()->deselect(lastSelected);
            }
        }

        selection.push_back(i);

        RBX::SelectionService::get()->selectInstance(i, false);
        CMainFrame::mainFrame->m_wndProperties.FillProperties(i);
    }
}

void CViewTree::ExpandAndFocusOnItem(RBX::Instance* i, HTREEITEM item)
{
    RBX::Instance* ancestor;
    ancestor = i;
    while (1)
    {
        if (ancestor->parent == 0)
            break;
        ancestor = ancestor->parent;
        Expand(CMainFrame::mainFrame->m_wndClassView.GetInstance(ancestor), TVE_EXPAND);
    }
}

void CViewTree::SelectInstance(RBX::Instance* instance)
{
    HTREEITEM item = CMainFrame::mainFrame->m_wndClassView.GetInstance(instance);

    if (!instance)
    {
        CMainFrame::mainFrame->m_wndProperties.ClearProperties();
    }
    
    if (item)
    {
        DeselectAll();
        ExpandAndFocusOnItem(instance, item);
        SelectWorldItem(item, 0);
    }
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CViewTree::OnTvnSelchanged)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

#pragma warning(suppress : 26454)
	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


void CViewTree::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = 0;
}


void CViewTree::OnKillFocus(CWnd* pNewWnd)
{
	CTreeCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	//CMainFrame::mainFrame->m_wndProperties.ClearProperties();
}


void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
    CTreeCtrl& treeCtrl = *this;

    UINT nHitFlags = 0;
    HTREEITEM hClickedItem = treeCtrl.HitTest(point, &nHitFlags);

    if (nHitFlags & LVHT_ONITEM)
    {
        // Find which item is currently selected
        HTREEITEM hSelectedItem = treeCtrl.GetSelectedItem();

        // Action depends on whether the user holds down the Shift or Ctrl key
        if (nFlags & MK_SHIFT)
        {
            // Is the clicked item above or below the selected item ?
            HTREEITEM hItem = treeCtrl.GetFirstVisibleItem();

            BOOL bReverse = FALSE;
            while (1)
            {
                if (hItem == hClickedItem)
                {
                    bReverse = TRUE;
                    break;
                }
                else if (hItem == hSelectedItem)
                    break;

                hItem = treeCtrl.GetNextVisibleItem(hItem);
            };

            // Select the clicked item (this will also deselect the previous one!)
            treeCtrl.SelectItem(hClickedItem);
            SelectWorldItem(hClickedItem, 1);

            // Now select all visible items between previously selected item
            // and the newly selected item
            hItem = hSelectedItem;
            do
            {
                treeCtrl.SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
                if (bReverse)
                    hItem = treeCtrl.GetPrevVisibleItem(hItem);
                else
                    hItem = treeCtrl.GetNextVisibleItem(hItem);
                SelectWorldItem(hItem, 1);
            } while (hItem != hClickedItem && hItem != NULL);
        }
        else if (nFlags & MK_CONTROL)
        {
            // Is the clicked item already selected ?
            BOOL bIsClickedItemSelected = treeCtrl.GetItemState(hClickedItem,
                TVIS_SELECTED) & TVIS_SELECTED;
            BOOL bIsSelectedItemSelected = treeCtrl.GetItemState(hSelectedItem,
                TVIS_SELECTED) & TVIS_SELECTED;

            // Select the clicked item (this will also deselect the previous one!)
            treeCtrl.SelectItem(hClickedItem);

            // If the previously selected item was selected, re-select it
            if (bIsSelectedItemSelected)
                treeCtrl.SetItemState(hSelectedItem, TVIS_SELECTED, TVIS_SELECTED);

            // We want the newly selected item to toggle its selected state,
            // so unselect now if it was already selected before
            if (bIsClickedItemSelected)
            {
                DeselectItem(hClickedItem);
                treeCtrl.SetItemState(hClickedItem, 0, TVIS_SELECTED);
            }
            else
            {
                SelectWorldItem(hClickedItem, 1);
                treeCtrl.SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);
            }
        }
        else
        {
            // Clear selection of all "multiple selected" items first

            HTREEITEM hItem = treeCtrl.GetFirstVisibleItem();
            RBX::SelectionService::get()->deselectAll();

            do
            {
                if (treeCtrl.GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                    treeCtrl.SetItemState(hItem, 0, TVIS_SELECTED);
                hItem = treeCtrl.GetNextVisibleItem(hItem);

            } while (hItem != NULL);

            // Then select the clicked item
            treeCtrl.SelectItem(hClickedItem);
            treeCtrl.SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);
            SelectWorldItem(hClickedItem, 0);
        }

        /* no matter what happens - grab focus*/
        SetFocus();
    }
    else
        // Behave normally
        CTreeCtrl::OnLButtonDown(nFlags, point);
}
