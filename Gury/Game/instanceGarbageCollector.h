#pragma once

#include "Objects/instance.h"
#include "Lua/singletonType.h"

namespace RBX
{
	class InstanceGarbageCollector : public Singleton<InstanceGarbageCollector>
	{
	private:

		Instances instances;

	public:

		void step();

		void addInstance(Instance* instance);

		InstanceGarbageCollector()
		{
			instances = Instances();
		}
	};
}