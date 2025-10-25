
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Objects/model.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Objects/mesh.h"

#include "../Gury/Game/Rendering/worldManager.h"

#include "../Gury/Application/appmanager.h"

#include "scene.h"

RBX::WorldScene* RBX::WorldScene::get()
{
	return RBX::Datamodel::get()->scene;
}

void RBX::WorldScene::close()
{
	for (Instance* object : sceneObjects)
	{
		object->remove();
	}
	sceneObjects.clear();
}

RBX::Instances RBX::WorldScene::getArrayOfObjects()
{
	return sceneObjects;
}

void RBX::WorldScene::updateSteppables()
{
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		ISteppable* steppableObject = toInstance<ISteppable>(sceneObjects.at(i));
		if (steppableObject)
		{
			steppableObject->onStep();
		}
	}
}

void RBX::WorldScene::updateSteppablesKernelly()
{
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		ISteppable* steppableObject = toInstance<ISteppable>(sceneObjects.at(i));
		if (steppableObject)
		{
			steppableObject->onKernelStep();
		}
	}
}

void RBX::WorldScene::initializeKernel()
{
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		PVInstance* pvInstance = toInstance<PVInstance>(sceneObjects.at(i));
		if (pvInstance)
		{
			pvInstance->initializeForKernel();
		}
	}
}

void RBX::WorldScene::saveStartPVs() /* before run: save each position of everything in the scene */
{
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		PVInstance* pvInstance = toInstance<PVInstance>(sceneObjects.at(i));
		if (pvInstance)
		{
			pvInstance->savePV();
		}
	}
}

void RBX::WorldScene::onWorkspaceDescendentAdded(Render::IRenderable* instance)
{
	if (IsA<Render::IRenderable>(instance) || IsA<RBX::ISteppable>(instance))
	{
		Render::SpecialMesh* specialMesh = toInstance<Render::SpecialMesh>(instance);
		PVInstance* pvInstance = toInstance<PVInstance>(instance);

		if (specialMesh)
		{
			RBX::Instance* parent = instance->parent;
			if (parent)
			{
				Render::IRenderable* renderable = (Render::IRenderable*)(parent);
				renderable->specialShape = instance;
			}
		}
		else
		{
			//instance->write();
			Render::WorldManager::get()->makeDirty();
		}

		if (pvInstance)
		{
			Gurnel::get()->addQueuedPrimitive(pvInstance->primitive);
		}

		sceneObjects.push_back(instance);
	}
	else if (IsA<ModelInstance>(instance))
	{
		ModelInstance* model = toInstance<ModelInstance>(instance);
		model->buildJoints();
	}
}

void RBX::WorldScene::onWorkspaceDescendentRemoved(RBX::Render::IRenderable* instance)
{
	if (std::find(sceneObjects.begin(), sceneObjects.end(), instance) != sceneObjects.end())
	{
		sceneObjects.erase(std::remove(
			sceneObjects.begin(),
			sceneObjects.end(),
			instance));

		PVInstance* pv = toInstance<PVInstance>(instance);
		if (pv)
		{
			/* Clear out surfaces + physical presence */
			pv->RemovePhysicalPresence();
			pv->removeSurfaces(pv->color);
		}

		instance->removeFromRenderEnvironment();
	}
}
