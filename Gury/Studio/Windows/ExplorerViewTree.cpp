
#include "../pch.h"
#include "../supergurymfc.h"
#include "../ViewTree.h"
#include "../MainFrm.h"

#include "../Gury/Main/framework.h"
#include "../Gury/Game/Services/stdout.h"
#include "../../Game/Objects/basescript.h"
#include "../Documents/scriptEditorDocument.h"

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

void CViewTree::TryDoubleClick(HTREEITEM item)
{
    RBX::Instance* instance = CMainFrame::mainFrame->m_wndClassView.GetTreeItem(item);

    /* if this is a script */

    RBX::BaseScript* script = RBX::toInstance<RBX::BaseScript>(instance);
    if (script) {

        /* then open in ide... */

        CDocument* document = theApp.pScriptDocTemplate->OpenDocumentFile(0);
        scriptEditDoc* ideDocument = dynamic_cast<scriptEditDoc*>(document);

        if (ideDocument) {
            ideDocument->attachedScript = script;
            ideDocument->update();
        }

    }
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CViewTree::OnTvnSelchanged)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
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

void CViewTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    UINT nHitFlags = 0;
    HTREEITEM hClickedItem = HitTest(point, &nHitFlags);

    if (nHitFlags & LVHT_ONITEM)
    {
        HTREEITEM hSelectedItem = GetSelectedItem();
        TryDoubleClick(hSelectedItem);
    }

    CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
    CClientDC dc(this);
    
    if (m_isDraggingItem)
    {
        if (hDraggedItem)
        {

        }
    }

    CTreeCtrl::OnMouseMove(nFlags, point);
}

void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isDraggingItem) /* drop item in parent */
    {
        UINT nHitFlags = 0;
        HTREEITEM hNewParent = HitTest(point, &nHitFlags);
        HTREEITEM hOldParent = GetNextItem(hDraggedItem, TVGN_PARENT);

        RBX::Instance* child = CMainFrame::mainFrame->m_wndClassView.GetTreeItem(hDraggedItem);
        RBX::Instance* newParent = CMainFrame::mainFrame->m_wndClassView.GetTreeItem(hNewParent);

        if (child && newParent)
        {
            if (child != newParent &&
                newParent != child->parent)
            {
                DeselectAll();
                child->setParent(newParent);
                SelectWorldItem(hNewParent, false);
            }
        }

        m_isDraggingItem = false;
        hDraggedItem = 0;
    }

    CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
    CTreeCtrl& treeCtrl = *this;

    UINT nHitFlags = 0;
    HTREEITEM hClickedItem = treeCtrl.HitTest(point, &nHitFlags);

    if (!m_isDraggingItem)
    {
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

                    m_isDraggingItem = true;
                    hDraggedItem = hClickedItem;
            }

            /* no matter what happens - grab focus*/
            SetFocus();
        }
        else
            // Behave normally
            CTreeCtrl::OnLButtonDown(nFlags, point);
    }
}
