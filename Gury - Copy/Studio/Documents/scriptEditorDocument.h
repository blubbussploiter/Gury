#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Main/Resource.h"

#include "../Gury/Game/Objects/basescript.h"

class scriptEditDoc : public CDocument
{
protected: // create from serialization only

	DECLARE_DYNCREATE(scriptEditDoc)
	scriptEditDoc() noexcept;

	// Attributes
public:

	RBX::BaseScript* attachedScript;

	// Operations
public:

	// Overrides
public:
	//	virtual BOOL OnNewDocument();
	// Implementation
public:
	virtual ~scriptEditDoc();
protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	void update();

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnNewDocument();
};