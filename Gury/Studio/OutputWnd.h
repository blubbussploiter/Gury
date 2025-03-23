
#pragma once

#include "pch.h"
#include "../Main/framework.h"

#include "../Gury/Game/log.h"

#include <string>
#include <afxhtml.h>

class COutputView : public CHtmlView
{
private:
	// Construction
public:
	COutputView() noexcept;

public:
	virtual ~COutputView();

	void WriteText(std::string preText, std::string text, std::string color);
	void WriteQuickText(std::string text);

protected:

	void OnInitialUpdate() override;

	DECLARE_MESSAGE_MAP()
};

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

	void AddText(RBX::MessageType messageType, std::string preText, std::string Text)
	{
		switch (messageType)
		{
			case RBX::MESSAGE_INFO:
			{
				m_wndOutput.WriteText(preText, Text, "blue");
				break;
			}
			case RBX::MESSAGE_ERROR:
			{
				m_wndOutput.WriteText(preText, Text, "red");
				break;
			}
			case RBX::MESSAGE_WARNING:
			{
				m_wndOutput.WriteText(preText, Text, "orange");
				break;
			}
		}
	}

// Attributes
protected:

	COutputView m_wndOutput;

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

