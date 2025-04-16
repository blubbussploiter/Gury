#include "scriptEditorDocument.h"
#include "scriptEditorView.h"

IMPLEMENT_DYNCREATE(scriptEditDoc, CDocument)

BEGIN_MESSAGE_MAP(scriptEditDoc, CDocument)
END_MESSAGE_MAP()

scriptEditDoc::scriptEditDoc() noexcept
{
	// TODO: add one-time construction code here

}

scriptEditDoc::~scriptEditDoc()
{ 
}

void scriptEditDoc::update()
{
	if (attachedScript) {

		CView* pView = nullptr;
		POSITION pos = GetFirstViewPosition();

		if (pos != nullptr)
		{
			pView = GetNextView(pos);
		}

		if (pView) {
			scriptEditView* view = dynamic_cast<scriptEditView*>(pView);
			if (view) {
				RBX::ProtectedString source = attachedScript->getSource();
				view->doUpdate(source.source);
			}
		}

		SetTitle(attachedScript->name.c_str());
	}
}

BOOL scriptEditDoc::OnSaveDocument(LPCTSTR lpszPathName) /* No save plz */
{
	return 1;
}

BOOL scriptEditDoc::OnNewDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	return CDocument::OnNewDocument();
}
