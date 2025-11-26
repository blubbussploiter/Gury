
#include "../Gury/Game/log.h"
#include "rbxhtml.h"

#define style "font-family:\"Tahoma\", sans-serif; font-size: calc(var(--total-scale-factor) *4.00px); transform: scale(1.5, 0.75);"

BEGIN_MESSAGE_MAP(CRBXHtmlView, CHtmlView)
END_MESSAGE_MAP()

CRBXHtmlView::CRBXHtmlView() noexcept
{

}

CRBXHtmlView::~CRBXHtmlView()
{
	//Stop();
	//OnQuit();
}

void CRBXHtmlView::WriteText(std::string preText, std::string text, std::string color)
{
	IHTMLDocument2* pDoc = 0;
	LPDISPATCH pDisp = GetHtmlDocument();

	if (pDisp && SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)))
	{
		IHTMLElement* pBody;
		pDoc->get_body(&pBody);

		if (pBody)
		{
			std::string bold = std::string();
			if (color != "blue")
			{
				bold = "font-weight:bold;";
			}
			else { 
				bold = "font-weight:normal;"; 
			}

			std::string html = RBX::Format("<span style='%s'>%s</span><span style='%s color:%s; %s; '>%s</span><br>", style, preText.c_str(), style, color.c_str(), bold.c_str(), text.c_str());

			BSTR bHtml = _bstr_t(html.c_str());
			pBody->insertAdjacentHTML(_bstr_t("beforeEnd"), bHtml);
			pBody->Release();
		}

		pDisp->Release();
		pDoc->close();
	}

}

void CRBXHtmlView::WriteQuickText(std::string text)
{
	IHTMLDocument2* pDoc = 0;
	LPDISPATCH pDisp = GetHtmlDocument();

	if (pDisp && SUCCEEDED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc)))
	{
		IHTMLElement* pBody;
		pDoc->get_body(&pBody);

		if (pBody)
		{
			std::string html = RBX::Format("<span style='%s'>%s</span>", style, text.c_str());

			BSTR bHtml = _bstr_t(html.c_str());
			pBody->insertAdjacentHTML(_bstr_t("beforeEnd"), bHtml);
			pBody->Release();
		}

		pDisp->Release();
		pDoc->close();
	}

}

void CRBXHtmlView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// Load a blank document
	Navigate2(_T("about:blank"), NULL, NULL);

}