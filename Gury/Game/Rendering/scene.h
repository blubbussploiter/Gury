#pragma once

#include "../Gury/Game/Objects/IRenderable.h"
#include "../Gury/Kernel/ISteppable.h"

#include "../Gury/Game/World/datamodel.h"

namespace RBX
{

	class WorldScene : public RBX::Instance
	{
	public:

		Instances sceneObjects;
		Instances sceneModels;

		void onWorkspaceDescendentAdded(RBX::Render::IRenderable* descendent);
		void onWorkspaceDescendentRemoved(RBX::Render::IRenderable* descendent);

		void updateSteppables();
		void updateSteppablesKernelly();

		void initializeKernel();
		void saveStartPVs();

		static bool isRenderable(RBX::Instance* instance) { return dynamic_cast<RBX::Render::IRenderable*>(instance) != 0; }

		static WorldScene* get();
		void close();

		WorldScene()
		{
			sceneObjects = Instances();
			sceneModels = Instances();
		}

		Instances getArrayOfObjects();
	};
}
