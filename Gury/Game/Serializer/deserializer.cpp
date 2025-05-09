#include <thread>
#include <iostream>

#include "serializer.h"

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Objects/part.h"

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/classes.h"
#include "../Gury/Game/Services/content.h"
#include "../Objects/basescript.h"

rapidxml::xml_document<> doc;

G3D::Table<std::string, RBX::Instance*> RBX::Serializer::rbxRefMap = {};

static std::map<std::string, int> xml_tokens =
{
	{ "Vector3", 0},
	{ "CFrame", 1},
	{ "CoordinateFrame", 1},
	{ "Color3", 3},
	{ "string", 4},
	{ "token", 5},
	{ "int", 6},
	{ "float", 7},
	{ "bool", 8},
	{ "Content", 9},
	{"ProtectedString", 10},
	{ "Ref", 11 },
};

bool RBX::Serializer::checkTag()
{
	return !strcmp(doc.first_node()->name(), "roblox");
}

Vector3 readVector3(rapidxml::xml_node<>* node)
{
	Vector3 v;
	v = Vector3::zero();

	float x, y, z;

	x = std::stof(node->first_node("X")->value());
	y = std::stof(node->first_node("Y")->value());
	z = std::stof(node->first_node("Z")->value());

	v = Vector3(x, y, z);

	return v;
}

Color3 readColor3(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<char>* r, * g, * b;

	r = node->first_node("R");
	g = node->first_node("G");
	b = node->first_node("B");

	if (r &&
		g &&
		b)
	{
		return Color3(std::stof(r->value()), std::stof(g->value()), std::stof(b->value()));
	}
	else
	{
		char* hex = node->value();
		if (hex)
		{
			return Color3::fromARGB(std::stoul(hex));
		}
	}

	return Color3::white();
}

CoordinateFrame readCFrame(rapidxml::xml_node<>* node)
{
	CoordinateFrame cf;
	float x, y, z, r00, r01, r02, r10, r11, r12, r20, r21, r22;

	x = std::stof(node->first_node("X")->value());
	y = std::stof(node->first_node("Y")->value());
	z = std::stof(node->first_node("Z")->value());
	r00 = std::stof(node->first_node("R00")->value());
	r01 = std::stof(node->first_node("R01")->value());
	r02 = std::stof(node->first_node("R02")->value());
	r10 = std::stof(node->first_node("R10")->value());
	r11 = std::stof(node->first_node("R11")->value());
	r12 = std::stof(node->first_node("R12")->value());
	r20 = std::stof(node->first_node("R20")->value());
	r21 = std::stof(node->first_node("R21")->value());
	r22 = std::stof(node->first_node("R22")->value());

	Matrix3 matrix;

	matrix = Matrix3(r00, r01, r02, r10, r11, r12, r20, r21, r22);
	cf = CoordinateFrame(matrix, Vector3(x, y, z));

	return cf;
}

RBX::Content getContent(rapidxml::xml_node<>* node)
{
	rapidxml::xml_node<>* binaryNode, * urlNode, * nullNode;

	binaryNode = node->first_node("binary");
	urlNode = node->first_node("url");
	nullNode = node->first_node("null");

	if (binaryNode)
	{
		return RBX::Content::fromBinary(binaryNode->value());
	}

	if (urlNode)
	{
		return RBX::Content::fromStoredContent(urlNode->value());
	}

	/* other, http stuff idk */

	return RBX::Content();
}

void setProperty(rapidxml::xml_node<>* node, RBX::Instance* instance, std::string propertyType, std::string propertyValue, std::string propertyName)
{
	rttr::type global_type = rttr::type::get_by_name(instance->getClassName());

	rttr::property property = global_type.get_property(propertyName);
	rttr::type type = property.get_type();

	if (property)
	{
		if (xml_tokens.count(propertyType))
		{
			switch (xml_tokens[propertyType])
			{
			case 0: /* Vector3 */
			{
				property.set_value(instance, readVector3(node));
				break;
			}
			case 1: /* CFrame */
			{
				property.set_value(instance, readCFrame(node));
				break;
			}
			case 3:
			{
				property.set_value(instance, readColor3(node));
				break;
			}
			case 4:
			{
				property.set_value(instance, propertyValue);
				break;
			}
			case 5:
			case 6:
			{
				try {
					int i = std::stoi(propertyValue);
					property.set_value(instance, i);
				}
				catch (...) {}
				break;
			}
			case 7:
			{
				property.set_value(instance, std::stof(propertyValue));
				break;
			}
			case 8:
			{
				property.set_value(instance, propertyValue == "true");
				break;
			}
			case 9:
			{
				property.set_value(instance, getContent(node));
				break;
			}
			case 10: /* ProtectedString not (usually) used, just convert to string */
			{
				property.set_value(instance, propertyValue);
				break;
			}
			case 11:
			{
				//RBX::Instance* referent = RBX::Serializer::rbxRefMap[propertyValue];
				//if (referent)
				//{
					//property.set_value(instance, referent);
				//}
			}
			}
		}

		if (type.is_enumeration())
		{
			rttr::enumeration enumeration = type.get_enumeration();
			const rttr::type enumType = enumeration.get_type();

			rttr::variant value;

			if (propertyType == "string")
			{
				value = propertyValue;
			}

			if (propertyType == "int")
			{
				value = std::stoi(propertyValue);
			}

			if (propertyType == "token")
			{
				try { value = std::stoi(propertyValue); }
				catch (...) { value = propertyValue; }
			}

			if (value.convert(enumType))
			{
				property.set_value(instance, value);
			}
		}
	}
}

void iteratePropertiesNode(rapidxml::xml_node<>* properties, RBX::Instance* instance)
{
	for (rapidxml::xml_node<>* prop = properties->first_node(); prop; prop = prop->next_sibling())
	{
		std::string propertyType;
		std::string propertyValue;
		std::string propertyName;

		propertyType = prop->name();
		propertyValue = prop->value();

		propertyName = prop->first_attribute()->value();

		setProperty(prop, instance, propertyType, propertyValue, propertyName);
	}
}

RBX::Instance* readInstance(rapidxml::xml_node<>* instanceNode, RBX::Instance* parent)
{
	RBX::Instance* inst = 0;
	if (!strcmp(instanceNode->name(), "Item"))
	{
		rapidxml::xml_attribute<>* classAttr = instanceNode->first_attribute("class");
		rapidxml::xml_attribute<>* referent = instanceNode->first_attribute("referent");
		if (classAttr)
		{
			std::string className = classAttr->value();
			inst = RBX::fromName(className);

			if (RBX::IsA<RBX::PartInstance>(inst)) /* no default studs, since empty surfaces means smooth */
			{
				RBX::PartInstance* part = RBX::toInstance<RBX::PartInstance>(inst);
				part->setTopSurface(RBX::Smooth);
				part->setBottomSurface(RBX::Smooth);
			}

			if (inst)
			{
				if (parent)
				{
					inst->setParent(parent);
				}
				rapidxml::xml_node<>* properties = instanceNode->first_node("Properties");
				if (properties)
				{
					iteratePropertiesNode(properties, inst);
				}
				if (referent)
				{
					std::string ref = referent->value();
					RBX::Serializer::rbxRefMap.set(ref, inst);
				}
			}

		}
	}
	return inst;
}

void iterateNode(rapidxml::xml_node<>* scanNode, RBX::Instance* parent, RBX::Instances* instances = 0)
{
	for (rapidxml::xml_node<>* node = scanNode->first_node(); node; node = node->next_sibling())
	{
		RBX::Instance* read = readInstance(node, parent);
		if (read)
		{
			if (instances && !parent)
			{
				instances->push_back(read);
			}
			iterateNode(node, read, instances);
		}
	}
}

void RBX::Serializer::load(std::string fileName)
{
	rapidxml::file<> file(fileName.c_str());

	if (file.size() <= 0)
		return;

	try
	{
		doc.parse<0>(file.data());
	}
	catch(std::exception e)
	{
		MessageBox(0, "Failed to open document", "Gury", MB_OK | MB_ICONWARNING);
		RBX::StandardOut::print(RBX::MESSAGE_ERROR, e.what());
		return;
	}

	if (!checkTag())
		return;

	iterateNode(doc.first_node(), 0);

	doc.clear();
	rbxRefMap.clear();
}

RBX::Instances* RBX::Serializer::loadInstances(std::string fileName)
{
	rapidxml::file<> file(fileName.c_str());
	RBX::Instances* i = new RBX::Instances();

	if (file.size() <= 0)
		return i;

	doc.parse<0>(file.data());

	if (!checkTag())
		return i;

	iterateNode(doc.first_node(), 0, i);
	doc.clear();
	
	rbxRefMap.clear();

	return i;
}

