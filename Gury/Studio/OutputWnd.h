
#pragma once


#include "pch.h"

#include "Windows/rbxhtml.h"
#include "../Gury/Game/log.h"

#include <string>

class COutputWnd : public CDockablePane
{
private:
// Construction
public:
	COutputWnd() noexcept;

	void UpdateFonts();	

	void SetTextColor(int r, int g, int b);

	void AddTextQuick(std::string text)
	{
		m_wndOutput.WriteQuickText(text);
	}

	void AddText(RBX::MessageType messageType, std::string preText, std::string Text);

// Attributes
protected:

	CRBXHtmlView m_wndOutput;

protected:

	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

