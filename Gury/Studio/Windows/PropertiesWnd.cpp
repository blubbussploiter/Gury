
#include <iomanip>
#include <sstream>
#include <G3D/Color3.h>

#include "../pch.h"

#include "../PropertiesWnd.h"
#include "../MainFrm.h"
#include "../supergurymfc.h"

#include "../Gury/Game/Objects/basescript.h"
#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/rbx.h"
#include "../../Main/Resource.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{

}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

// Gury base property things 

void setProperty(rttr::variant v, RBX::Instance* object, CMFCPropertyGridProperty*& parent, RobloxProperty*& property, std::string name, bool readonly)
{
	rttr::type t = v.get_type();
	std::string propertyValue;

	if (t == rttr::type::get<std::string>())
	{
		propertyValue = v.to_string();
	}

	if (t == rttr::type::get<bool>())
	{
		v.to_bool() ? propertyValue = "False" : propertyValue = "True";
		property = new RobloxProperty(name.c_str(), (_variant_t)v.to_bool(), "");
	}

	if (t == rttr::type::get<float>())
	{
		propertyValue = RBX::decimalToString(v.to_float());
	}

	if (t == rttr::type::get<double>())
	{
		propertyValue = RBX::decimalToString(v.to_double());
	}

	if (t == rttr::type::get<int>())
	{
		propertyValue = RBX::Format("%d", v.to_int());
	}

	if (t == rttr::type::get<G3D::Vector3>())
	{
		Vector3 value = v.convert<G3D::Vector3>();

		property = new RobloxProperty(name.c_str(), 0, 1);
		property->object = object;

		parent->AddSubItem(property);
		property->AddSubItem(new RobloxProperty("x", RBX::decimalToString(value.x).c_str(), ""));
		property->AddSubItem(new RobloxProperty("y", RBX::decimalToString(value.y).c_str(), ""));
		property->AddSubItem(new RobloxProperty("z", RBX::decimalToString(value.z).c_str(), ""));
		return;
	}

	if (t == rttr::type::get<RBX::ProtectedString>())
	{
		RBX::ProtectedString string = v.convert<RBX::ProtectedString>();
		propertyValue = string.source;
	}

	if (t == rttr::type::get<RBX::Content>()) {
		RBX::Content content = v.convert<RBX::Content>();
		if (content.isBinary) {
			propertyValue = content.binary;
		}
		else if (content.isStored || content.isOnline) {
			propertyValue = content.contentUrl;
		}
	}

	if (t == rttr::type::get<G3D::Color3>())
	{
		/* Make our own color property thing */
		G3D::Color3 c = v.convert<G3D::Color3>();
		property = new RobloxProperty(name.c_str(), RGB(c.r * 255, c.g * 255, c.b * 255));
	}

	if (t.is_enumeration())
	{
		rttr::enumeration e = t.get_enumeration();
		rttr::string_view sv = e.value_to_name(v);
		std::string s = sv.to_string();

		property = new RobloxProperty(name.c_str(), (_variant_t)s.c_str(), "");

		for (auto& val : e.get_names())
		{
			std::string str = val.to_string();
			property->AddOption(str.c_str());
		}
	}

	/* For number, bools, strings */

	if (!property)
	{
		property = new RobloxProperty(name.c_str(), (_variant_t)propertyValue.c_str(), "");
	}

	if (property)
	{
		property->object = object;
		if (readonly)
		{
			property->Enable(0);
		}
		parent->AddSubItem(property);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + 0, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - (cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::ClearProperties()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();
	catergories.clear();
}

void CPropertiesWnd::FillProperties(RBX::Instance* i)
{
	rttr::type base = rttr::detail::get_type_from_instance(i);
	rttr::type type = rttr::type::get_by_name(i->getClassName());

	if (!type) type = base;

	ClearProperties();

	for (auto& p : type.get_properties())
	{
		AddProperty(i, p);
	}

	m_wndPropList.RedrawWindow();
}

void CPropertiesWnd::AddProperty(RBX::Instance* i, rttr::property p)
{
	rttr::variant var = p.get_metadata("Type");
	if (!var.is_valid()) return;

	std::string name = p.get_name().to_string();
	RBX::Types type = var.convert<RBX::Types>();

	CMFCPropertyGridProperty* catergory = catergories[type];
	RobloxProperty* property = 0;

	rttr::variant v = p.get_value(i);

	if (!catergory)
	{
		catergory = new CMFCPropertyGridProperty(RBX::getType(type).c_str());

		m_wndPropList.AddProperty(catergory);
		catergories[type] = catergory;

		catergory->Expand();
	}

	setProperty(v, i, catergory, property, name, p.is_readonly());

	m_wndPropList.AdjustLayout();
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.SetVSDotNetLook(1);
	m_wndPropList.MarkModifiedProperties();

	m_wndPropList.EnableHeaderCtrl(0);
	m_wndPropList.EnableScrollBar(SB_HORZ);

	m_wndPropList.EnableDescriptionArea(0);

	COLORREF background, text, groupBackground, groupText, descriptionBackground, descriptionText, line;

	m_wndPropList.GetCustomColors(background, text, groupBackground, groupText, descriptionBackground, descriptionText, line);
	m_wndPropList.SetCustomColors(background, text, groupBackground, RGB(0,0,0), descriptionBackground, descriptionText, line);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}