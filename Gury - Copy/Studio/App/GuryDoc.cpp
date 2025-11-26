
// supergurymfcDoc.cpp : implementation of the CsupergurymfcDoc class
//

#include "../pch.h"
#include "../../Main/framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "../supergurymfc.h"
#endif

#include "../supergurymfcDoc.h"
#include "../MainFrm.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CsupergurymfcDoc

IMPLEMENT_DYNCREATE(GuryDoc, CDocument)

BEGIN_MESSAGE_MAP(GuryDoc, CDocument)
END_MESSAGE_MAP()


// CsupergurymfcDoc construction/destruction

GuryDoc::GuryDoc() noexcept
{
	// TODO: add one-time construction code here

}

GuryDoc::~GuryDoc()
{
}

// CsupergurymfcDoc serialization

void GuryDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
