#pragma once

#include "../Gury/Main/framework.h"
#include "../Gury/Main/Resource.h"

class htmlPageDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_DYNCREATE(htmlPageDoc)
	htmlPageDoc() noexcept;

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	//	virtual BOOL OnNewDocument();
	// Implementation
public:
	virtual ~htmlPageDoc();
protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};