
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

		BrickColor brickColor;

		void setBrickColor(int bc)
		{
			brickColor = BrickColor::BrickMap::get()->brickColorFromNumber(bc);
			setColor(brickColor.color);
		}

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
			size = Vector3(2, 1.2f, 4);

		}

		RBX_CLONE_DEF(PartInstance) 
		RTTR_ENABLE(RBX::PVInstance)
	};
}
