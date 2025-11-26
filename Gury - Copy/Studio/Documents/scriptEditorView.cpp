#include <Scintilla/Scintilla.h>
#include <Scintilla/SciLexer.h>

#include "scriptEditorView.h"

static const char luaKeywords[] =
"and break do else elseif end false for function if in local nil not or repeat return then true until while";

IMPLEMENT_DYNCREATE(scriptEditView, CView)

BEGIN_MESSAGE_MAP(scriptEditView, CView)
	ON_WM_CREATE()
	ON_WM_SIZING()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_EDIT_UNDO, &scriptEditView::onUndo)
	ON_COMMAND(ID_EDIT_REDO, &scriptEditView::onRedo)
	ON_COMMAND(ID_EDIT_CUT, &scriptEditView::onCut)
	ON_COMMAND(ID_EDIT_COPY, &scriptEditView::onCopy)
	ON_COMMAND(ID_EDIT_PASTE, &scriptEditView::onPaste)
	ON_COMMAND(ID_EDIT_CLEAR, &scriptEditView::onClear)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &scriptEditView::onSelectAll)
	ON_COMMAND(ID_FILE_SAVE, &scriptEditView::onSave)
//	ON_WM_APPCOMMAND()
ON_WM_SETFOCUS()
END_MESSAGE_MAP()

scriptEditView::scriptEditView() noexcept
{
}

void scriptEditView::doUpdate(std::string scriptContent)
{
	if (IsWindow(scintilla)) {
		::SendMessageA(scintilla, SCI_SETTEXT, 0, (LPARAM)scriptContent.c_str());
	}
}

void scriptEditView::doCommand(int id)
{
	switch (id) {
		case ID_EDIT_UNDO:
		{
			::SendMessageA(scintilla, SCI_UNDO, 0, 0);
			break;
		}
		case ID_EDIT_REDO:
		{
			::SendMessageA(scintilla, SCI_REDO, 0, 0);
			break;
		}
		case ID_EDIT_CUT:
		{
			::SendMessageA(scintilla, SCI_CUT, 0, 0);
			break;
		}
		case ID_EDIT_COPY:
		{
			::SendMessageA(scintilla, SCI_COPY, 0, 0);
			break;
		}
		case ID_EDIT_PASTE:
		{
			::SendMessageA(scintilla, SCI_PASTE, 0, 0);
			break;
		}
		case ID_EDIT_CLEAR:
		{
			::SendMessageA(scintilla, SCI_CLEAR, 0, 0);
			break;
		}
		case ID_EDIT_SELECT_ALL:
		{
			::SendMessageA(scintilla, SCI_SELECTALL, 0, 0);
			break;
		}
	};
}

void scriptEditView::onNotify(int notification, LPARAM lParam)
{
	switch (notification)
	{
		case SCN_MODIFIED:
		{
			int length = ::SendMessage(scintilla, SCI_GETLENGTH, 0, 0)+1;

			char* buffer = new char[length];
			::SendMessage(scintilla, SCI_GETTEXT, length, (LPARAM)buffer);

			scriptEditDoc* document = GetDocument();
			if (document)
			{
				if (document->attachedScript)
				{
					document->attachedScript->setSourceFromString(buffer);
				}
			}

			break;
		}
		case SCN_MARGINCLICK:
		{
			SCNotification* scn = (SCNotification*)lParam;
			if (scn->margin == 1) {
				int line = ::SendMessageA(scintilla, SCI_LINEFROMPOSITION, scn->position, 0);
				::SendMessage(scintilla, SCI_TOGGLEFOLD, line, 0);
			}
		}
	}
}

void scriptEditView::setAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face)
{
	::SendMessageA(scintilla, SCI_STYLESETFORE, style, fore);
	::SendMessageA(scintilla, SCI_STYLESETBACK, style, back);
	if (size >= 1)
		::SendMessageA(scintilla, SCI_STYLESETSIZE, style, size);
	if (face)
		::SendMessageA(scintilla, SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

void scriptEditView::OnDraw(CDC* pDC)
{
	scriptEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

BOOL scriptEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

scriptEditView::~scriptEditView()
{
}

BOOL scriptEditView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void scriptEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	HMODULE lexer = LoadLibrary("SciLexer.DLL");
	if (lexer == NULL)
	{
		MessageBox(
			"Failed to load IDE",
			"Error loading SciLexer.DLL",
			MB_OK | MB_ICONERROR);
	}

	CRect rectangle;
	GetWindowRect(&rectangle);

	scintilla = CreateWindowExA(0,
		"Scintilla", "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		0, 0, rectangle.Width(), rectangle.Height(), GetSafeHwnd(), 0, 0, NULL);

	::SendMessage(scintilla, SCI_SETLEXER, SCLEX_LUA, 0);
	::SendMessage(scintilla, SCI_SETSTYLEBITS, 7, 0);

	::SendMessage(scintilla, SCI_SETKEYWORDS, 0, (LPARAM)luaKeywords);

	::SendMessage(scintilla, SCI_CREATEDOCUMENT, 0, 0);
	::SendMessage(scintilla, SCI_STYLECLEARALL, 0, 0);
	::SendMessage(scintilla, SCI_SETZOOM, 2, 0);
	::SendMessage(scintilla, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
	::SendMessage(scintilla, SCI_LINESSPLIT, 1, 0);

	::SendMessage(scintilla, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
	::SendMessage(scintilla, SCI_SETMARGINLEFT, 0, 12);
	::SendMessage(scintilla, SCI_SETMARGINWIDTHN, 0 , 48);

	::SendMessage(scintilla, SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
	::SendMessage(scintilla, SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"1");

	::SendMessage(scintilla, SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
	::SendMessage(scintilla, SCI_SETMARGINWIDTHN, 1, 16);
	::SendMessage(scintilla, SCI_SETMARGINMASKN, 1, SC_MASK_FOLDERS);

	::SendMessage(scintilla, SCI_SETMARGINSENSITIVEN, 1, TRUE);
	::SendMessage(scintilla, SCI_SETFOLDFLAGS, 16, 0);
	::SendMessage(scintilla, SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT, 0);

	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PLUS);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
	::SendMessage(scintilla, SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);

	setAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(255, 255, 255), 11, "Verdana");
	setAStyle(STYLE_LINENUMBER, RGB(0, 0, 0), RGB(240, 240, 240), 8, "Verdana");

	setAStyle(SCE_LUA_NUMBER, RGB(0, 128, 128));
	setAStyle(SCE_LUA_WORD, RGB(0, 0, 128));
	::SendMessage(scintilla, SCI_STYLESETBOLD, SCE_LUA_WORD, 1);
	setAStyle(SCE_LUA_COMMENT, RGB(0, 128, 0));
	setAStyle(SCE_LUA_OPERATOR, RGB(128, 128, 0));
	setAStyle(SCE_LUA_CHARACTER, RGB(128, 0, 0));
	setAStyle(SCE_LUA_STRING, RGB(128, 0, 151));
	setAStyle(SCE_LUA_COMMENT, RGB(58, 128, 0));
	setAStyle(SCE_LUA_COMMENTLINE, RGB(58, 128, 0));
	setAStyle(SCE_LUA_COMMENTDOC, RGB(58, 128, 0));

}

void scriptEditView::OnSizing(UINT fwSide, LPRECT pRect)
{
	CView::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
}

void scriptEditView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CView::OnWindowPosChanging(lpwndpos);
	// TODO: Add your message handler code here
	
	if (scintilla)
	{
		::MoveWindow(scintilla, 0, 0, lpwndpos->cx, lpwndpos->cy, 1);
	}

}

//void scriptEditView::OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey)
//{
//	doCommand(nCmd);
//	CView::OnAppCommand(pWnd, nCmd, nDevice, nKey);
//}

BOOL scriptEditView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	UINT id = LOWORD(wParam);
	doCommand(id);

	return CView::OnCommand(wParam, lParam);
}

BOOL scriptEditView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	NMHDR* notification = (NMHDR*)(lParam);
	onNotify(notification->code, lParam);

	return CView::OnNotify(wParam, lParam, pResult);
}

void scriptEditView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	scriptEditDoc* pDoc = GetDocument();
	if (pDoc)
	{
		RBX::BaseScript* script = pDoc->attachedScript;
		if (script)
		{
			doUpdate(script->getSource().source);
		}
	}

	// TODO: Add your message handler code here
}
