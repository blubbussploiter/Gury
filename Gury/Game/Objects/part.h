
#pragma once

#include "../Gury/Game/Objects/PVInstance/pvinstance.h"
#include "../Gury/Game/Rendering/brickcolor.h"
#include "../Gury/Game/Reflection/enum.h"

namespace RBX
{

	class PartInstance :
		public RBX::PVInstance
	{
	public:

		void setBrickColor(int bc);

		int getBrickColor()
		{
			return brickColor.number.number;
		}

		void write();
		void writeDecals();
		void edit();

		virtual ~PartInstance() {}

		PartInstance()
		{
			setClassName("Part");
			setName("Part");

			shape = Shape::Block;
			setSize(Vector3(4, 1.2f, 2));

		}

		RBX_CLONE_DEF(PartInstance) 
		RTTR_ENABLE(RBX::PVInstance)
	};
}
