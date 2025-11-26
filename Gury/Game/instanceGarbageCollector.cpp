#include "instanceGarbageCollector.h"
#include "Rendering/scene.h"

DEFINE_SINGLETON(RBX::InstanceGarbageCollector);

void RBX::InstanceGarbageCollector::step()
{
	for (size_t i = 0; i < instances.size(); i++)
	{
		Instance* at = instances.at(i);
		if (at)
		{
			if (IsA<Render::IRenderable>(at))
			{
				WorldScene::get()->removeRenderable(RBXCast<Render::IRenderable>(at));
			}
			delete at;
		}
	}

	instances.clear();
}

void RBX::InstanceGarbageCollector::addInstance(Instance* instance)
{
	instances.push_back(instance);
}
