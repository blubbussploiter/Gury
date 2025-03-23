
#include <locale>
#include <codecvt>

#include <G3DAll.h>

#include "pch.h"

#include "PropertiesWnd.h"
#include "MainFrm.h"

#include "../Gury/Main/framework.h"
#include "../Gury/Game/Services/stdout.h"

std::string ass(BSTR wideStr)
{
	std::wstring wstr(wideStr);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

std::string getVariantValue(_variant_t v)
{
	if (v.vt == VT_BSTR)
	{
		return ass(v.bstrVal);
	}
	return "";
}

void updatePropertyValue(RobloxProperty* rbxp, rttr::property p, RBX::Instance* o, _variant_t v)
{
	rttr::type type = p.get_value(o).get_type();
	std::string value = getVariantValue(v);

	if (type == rttr::type::get<int>())
	{
		p.set_value(o, std::stoi(value));
	}

	if (type == rttr::type::get<float>())
	{
		p.set_value(o, std::stof(value));
	}

	if (type == rttr::type::get<double>())
	{
		p.set_value(o, std::stod(value));
	}

	if (type == rttr::type::get<std::string>())
	{
		p.set_value(o, value);
	}

	if (type == rttr::type::get<bool>())
	{
		/* why cant it just be 0 or 1 */
		if (v.boolVal == -1)
		{
			p.set_value(o, true);
		}
		else if (v.boolVal == 0)
		{
			p.set_value(o, false);
		}
	}

	if (type == rttr::type::get<Vector3>())
	{
		COleVariant x = rbxp->GetSubItem(0)->GetValue();
		COleVariant y = rbxp->GetSubItem(1)->GetValue();
		COleVariant z = rbxp->GetSubItem(2)->GetValue();

		if(x.vt == VT_BSTR && 
			y.vt == VT_BSTR &&
			z.vt == VT_BSTR)
		{

			Vector3 nv;

			nv.x = std::stof(ass(x.bstrVal));
			nv.y = std::stof(ass(y.bstrVal));
			nv.z = std::stof(ass(z.bstrVal));

			p.set_value(o, nv);

		}

	}

	if (type.is_enumeration())
	{
		rttr::enumeration enumeration = type.get_enumeration();
		p.set_value(o, enumeration.name_to_value(value));
	}
}

BOOL RobloxProperty::OnEndEdit()
{

	_variant_t value = GetValue();

	std::string propertyName = GetName();

	if(object)
	{
		rttr::type base = rttr::detail::get_type_from_instance(object);
		rttr::type type = rttr::type::get_by_name(object->getClassName());

		if (!type)
		{
			type = base;
		}

		rttr::property property = type.get_property(propertyName);

		if (!property.is_readonly())
		{
			updatePropertyValue(this, property, object, value);
		}
		 
		CMainFrame::mainFrame->m_wndProperties.ClearProperties();
		CMainFrame::mainFrame->m_wndProperties.FillProperties(object);
	}

	return CMFCPropertyGridProperty::OnEndEdit();
}