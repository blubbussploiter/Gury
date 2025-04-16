#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Main/Resource.h"

#include "scriptEditorDocument.h"

class scriptEditView : public CView
{
protected: // create from serialization only
	DECLARE_DYNCREATE(scriptEditView)
	scriptEditView() noexcept;


	// Attributes
public:

	HWND scintilla;

	scriptEditDoc* GetDocument() const;

	// Operations
public:

	afx_msg void onSave()
	{

	}

	afx_msg void onUndo() {
		doCommand(ID_EDIT_UNDO);
	}

	afx_msg void onRedo() {
		doCommand(ID_EDIT_REDO);
	}

	afx_msg void onCopy() {
		doCommand(ID_EDIT_COPY);
	}

	afx_msg void onCut() {
		doCommand(ID_EDIT_CUT);
	}

	afx_msg void onPaste() {
		doCommand(ID_EDIT_PASTE);
	}

	afx_msg void onClear() {
		doCommand(ID_EDIT_CLEAR);
	}

	afx_msg void onSelectAll() {
		doCommand(ID_EDIT_SELECT_ALL);
	}

	void doUpdate(std::string scriptContent);
	void doCommand(int id);
	void onNotify(int notification);
	void setAStyle(int style, COLORREF fore, COLORREF back = RGB(0xff, 0xff, 0xff), int size = -1, const char* face = 0);

	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	// Implementation
public:
	virtual ~scriptEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

#ifndef _DEBUG  // debug version in supergurymfcView.cpp
inline scriptEditDoc* scriptEditView::GetDocument() const
{
	return reinterpret_cast<scriptEditDoc*>(m_pDocument);
}
#endif

