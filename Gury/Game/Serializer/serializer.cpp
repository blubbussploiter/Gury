
#include <thread>
#include <fstream>

#include "serializer.h"

#include "rapidxml/rapidxml_print.hpp"

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/World/sounds.h"
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Services/soundservice.h"
#include "../Gury/Game/Services/selection.h"
#include "../Gury/Game/Network/Player/players.h"

#define XML_(str) (doc.allocate_string(str.c_str()))

int refCount = 0;

void writeInstance(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parentNode, RBX::Instance* instance);

std::string getToken(rttr::type type)
{
    if (type == rttr::type::get<Vector3>())
    {
        return "Vector3";
    }
    if (type == rttr::type::get<CoordinateFrame>())
    {
        return "CoordinateFrame";
    }
    if (type == rttr::type::get<Color3>())
    {
        return "Color3";
    }
    if (type == rttr::type::get<std::string>())
    {
        return "string";
    }
    if (type == rttr::type::get<int>())
    {
        return "int";
    }
    if (type == rttr::type::get<float>())
    {
        return "float";
    }
    if (type == rttr::type::get<bool>())
    {
        return "bool";
    }
    if (type.is_derived_from(rttr::type::get<RBX::Instance>()))
    {
        return "Ref";
    }
    if (type == rttr::type::get<RBX::Content>())
    {
        return "Content";
    }
    if (type == rttr::type::get<RBX::ProtectedString>())
    {
        return "ProtectedString";
    }
    return "token";
}

void writeCoordinateFrame(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* propertyNode, CoordinateFrame coordinateFrame)
{
    Vector3 vector3 = coordinateFrame.translation;
    Matrix3 matrix3 = coordinateFrame.rotation;
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "X", XML_(std::to_string(vector3.x))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "Y", XML_(std::to_string(vector3.y))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "Z", XML_(std::to_string(vector3.z))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R00", XML_(std::to_string(matrix3.elt[0][0]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R01", XML_(std::to_string(matrix3.elt[0][1]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R02", XML_(std::to_string(matrix3.elt[0][2]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R10", XML_(std::to_string(matrix3.elt[1][0]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R11", XML_(std::to_string(matrix3.elt[1][1]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R12", XML_(std::to_string(matrix3.elt[1][2]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R20", XML_(std::to_string(matrix3.elt[2][0]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R21", XML_(std::to_string(matrix3.elt[2][1]))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "R22", XML_(std::to_string(matrix3.elt[2][2]))));
}

void writeVector3(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* propertyNode, Vector3 vector3)
{
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "X", XML_(std::to_string(vector3.x))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "Y", XML_(std::to_string(vector3.y))));
    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "Z", XML_(std::to_string(vector3.z))));
}

void writeProperty(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* propertiesNode, rttr::property property, RBX::Instance* instance)
{
    std::string name = property.get_name().to_string();

    rttr::type type = property.get_type();
    rttr::variant variant = property.get_value(instance);

    rttr::variant serializable = property.get_metadata("Serializable");

    if (!serializable.is_valid()) /* Dont write non serializable properties */
    {
        rapidxml::xml_node<>* propertyNode;

        std::string token = getToken(type);
        std::string value;

        propertyNode = doc.allocate_node(rapidxml::node_element, XML_(token));

        if (type == rttr::type::get<Vector3>())
        {
            writeVector3(doc, propertyNode, variant.get_value<Vector3>());
        }

        if (type == rttr::type::get<CoordinateFrame>())
        {
            writeCoordinateFrame(doc, propertyNode, variant.get_value<CoordinateFrame>());
        }

        if (type == rttr::type::get<Color3>())
        {
            Color3 color = variant.get_value<Color3>();
            Color4uint8 uc = Color4uint8(color);
            value = XML_(RBX::Format("%u", uc.asUInt32()));
        }

        if (type == rttr::type::get<RBX::ProtectedString>())
        {
            RBX::ProtectedString string = variant.get_value<RBX::ProtectedString>();
            value = XML_(string.source);
        }

        if (type == rttr::type::get<RBX::Content>())
        {
            RBX::Content content = variant.get_value<RBX::Content>();
            if (content.isEmpty)
            {
                propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "null"));
            }
            else
            {
                if (content.isBinary)
                {
                    /* Write base64 */

                    std::string encoded = base64_encode(content.content, content.contentLength);
                    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "binary", XML_(encoded)));

                }
                else if (content.isStored)
                {
                    /* Write url or whatever */

                    propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "url", XML_(content.contentUrl)));
                }
            }
        }

        if (type.is_derived_from(rttr::type::get<RBX::Instance>()))
        {
            RBX::Instance* instance = variant.get_value<RBX::Instance*>();
            value = XML_(RBX::Serializer::getReferent(instance));
        }

        if (type.is_enumeration())
        {
            /* Write as int */
            int e = variant.to_int();
            value = XML_(RBX::decimalToString(e));
        }

        /* All string convertable types */

        if (type == rttr::type::get<std::string>() || 
            type == rttr::type::get<bool>() ||
            type == rttr::type::get<float>() || 
            type == rttr::type::get<int>())
        {
            value = XML_(variant.to_string());
        }

        /* Null */

        if (!variant.is_valid())
        {
            propertyNode->append_node(doc.allocate_node(rapidxml::node_element, "null"));
        }

        if (!value.empty())
        {
            propertyNode = doc.allocate_node(rapidxml::node_element, XML_(token), XML_(value));
        }
        propertyNode->append_attribute(doc.allocate_attribute("name", XML_(name)));

        propertiesNode->append_node(propertyNode);
    }
}

void writeProperties(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* itemNode, RBX::Instance* instance)
{
    /* Make Properties node */

    rapidxml::xml_node<>* propertiesNode = doc.allocate_node(rapidxml::node_element, "Properties");

    /* Write properties */

    rttr::type global_type = rttr::type::get_by_name(instance->getClassName());

    rttr::array_range<rttr::property> properties = global_type.get_properties();
    for (rttr::property property : properties)
    {
        writeProperty(doc, propertiesNode, property, instance);
    }

    itemNode->append_node(propertiesNode);
}

void writeInstance(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parentNode, RBX::Instance* instance)
{
    if (instance->archivable && 
        !RBX::Serializer::hasReferent(instance))
    {

        rapidxml::xml_node<>* item = doc.allocate_node(rapidxml::node_element, "Item");
        std::string referent = RBX::Format("RBX%d", refCount++); /* Sloppy! */

        /* Instance + referent */

        RBX::Serializer::rbxRefMap.set(referent, instance);

        item->append_attribute(doc.allocate_attribute("class", XML_(instance->className)));
        item->append_attribute(doc.allocate_attribute("referent", XML_(referent)));

        /* Children */

        for (size_t i = 0; i < instance->children->size(); i++)
        {
            writeInstance(doc, item, instance->children->at(i));
        }

        parentNode->append_node(item);

    }
}

rapidxml::xml_node<>* writeHead(rapidxml::xml_document<>& doc)
{
    rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_element, "roblox");
    decl->append_attribute(doc.allocate_attribute("xmlns:xmime", "http://www.w3.org/2005/05/xmlmime"));
    decl->append_attribute(doc.allocate_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"));
    decl->append_attribute(doc.allocate_attribute("xsi:noNamespaceSchemaLocation", "http://www.roblox.com/roblox.xsd"));
    decl->append_attribute(doc.allocate_attribute("version", "4"));
    decl->append_node(doc.allocate_node(rapidxml::node_element, "External", "null"));
    decl->append_node(doc.allocate_node(rapidxml::node_element, "External", "nil"));
    doc.append_node(decl);
    return decl;
}

void recursivePropertyWrite(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* robloxNode)
{
    for (rapidxml::xml_node<>* child = robloxNode->first_node(); child; child = child->next_sibling())
    {
        if (!strcmp(child->name(), "Item")) /* Is an Instance */
        {
            rapidxml::xml_attribute<>* referent = child->first_attribute("referent");
            RBX::Instance* referentInstance = RBX::Serializer::rbxRefMap[referent->value()];
            if (referentInstance)
            {
                writeProperties(doc, child, referentInstance);
            }
        }
        recursivePropertyWrite(doc, child);
    }

}

void RBX::Serializer::save(std::string fileName)
{
    std::ofstream file(fileName);
    rapidxml::xml_document<> doc;

    rapidxml::xml_node<>* roblox = writeHead(doc);

    refCount = 0;

    writeInstance(doc, roblox, RBX::Workspace::get());
    writeInstance(doc, roblox, RBX::RunService::get());
    writeInstance(doc, roblox, RBX::Network::Players::get());
    writeInstance(doc, roblox, RBX::SoundService::get());
    writeInstance(doc, roblox, RBX::SelectionService::get());
    writeInstance(doc, roblox, RBX::Lighting::get());

    /* Write properties - all referents have been written so no problem! */
    recursivePropertyWrite(doc, roblox);

    file << doc;

    file.close();
    rbxRefMap.clear();
}