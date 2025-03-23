#pragma once

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/World/datamodel.h"

namespace RBX
{
	template <typename T>
	class Service
	{
	public:
		Service()
		{
			((T*)(this))->setParent(Datamodel::get());
			((T*)(this))->isParentLocked = 1;
		}
	};
}