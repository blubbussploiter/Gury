#pragma once

#include "../pch.h"
#include "../Gury/Main/framework.h"

#include <string>
#include <afxhtml.h>

class CRBXHtmlView : public CHtmlView
{
private:
	// Construction
public:
	CRBXHtmlView() noexcept;

public:
	virtual ~CRBXHtmlView();

	void WriteText(std::string preText, std::string text, std::string color);
	void WriteQuickText(std::string text);

protected:

	void OnInitialUpdate() override;

	DECLARE_MESSAGE_MAP()
};
