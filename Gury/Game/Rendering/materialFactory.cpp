#include "materialFactory.h"

#include "../Gury/Game/Objects/part.h"

RBX::Render::MaterialFactory<RBX::PartInstance>* partFactory;

RBX::Render::MaterialFactory<RBX::PartInstance>* RBX::Render::MaterialFactory<RBX::PartInstance>::factory()
{
	if (!partFactory)
	{
		partFactory = new RBX::Render::MaterialFactory<RBX::PartInstance>();
		partFactory->getMaterial()->appendLevel(Color3::white(), Color3::white(), 20, 0.f, 0.f); // Opaque level
		partFactory->getMaterial()->appendLevel(Color3::white(), Color3::white(), 20, 0.f, 1.f); // Transparent level
		partFactory->getMaterial()->appendLevel(Color3::white(), Color3::white(), 20, 0.f, 1.f); // Textured Level
		partFactory->getMaterial()->appendLevel(Color3::white(), Color3::white(), 20, 1.f, 0.f); // Reflectant level
	}
	return partFactory;
}
