#pragma once

#include <string>

#include "G3D/Table.h"
#include "G3D/Array.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "../Gury/Game/Objects/instance.h"

namespace RBX
{
	class Serializer
	{
	private:
		static bool checkTag();
	public:

		static G3D::Table<std::string, RBX::Instance*> rbxRefMap;

		static bool hasReferent(RBX::Instance* instance)
		{
			G3D::Array<std::string> keys = rbxRefMap.getKeys();

			for (int i = 0; i < keys.size(); i++)
			{
				Instance* _i = rbxRefMap[keys[i]];
				if (_i && _i == instance)
				{
					return true;
				}
			}
			return false;
		}

		static std::string getReferent(RBX::Instance* instance)
		{
			G3D::Array<std::string> keys = rbxRefMap.getKeys();

			for (int i = 0; i < keys.size(); i++)
			{
				Instance* _i = rbxRefMap[keys[i]];
				if (_i && _i == instance)
				{
					return keys[i];
				}
			}
			return "null";
		}

		static void save(std::string fileName);

		static void load(std::string fileName);
		static RBX::Instances* loadInstances(std::string fileName);
	};
}