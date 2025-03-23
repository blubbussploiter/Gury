#include "classes.h"

#include "../Gury/Game/Objects/part.h"
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Objects/Humanoid/humanoid.h"
#include "../Gury/Game/Objects/basescript.h"
#include "../Gury/Game/Objects/Bins/hopperbin.h"
#include "../Gury/Game/Objects/decal.h"
#include "../Gury/Game/Rendering/texture.h"
#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Objects/mesh.h"

RBX::Instance* RBX::fromName(std::string className)
{
	/* replace with a template, or something */

	if (className == "Part")
	{
		PartInstance* partInstance = new RBX::PartInstance();
		return partInstance;
	}

	if (className == "Model")
	{
		return new RBX::ModelInstance();
	}

	if (className == "SpecialMesh")
	{
		return new RBX::Render::SpecialMesh();
	}

	if (className == "Humanoid")
	{
		return new RBX::Humanoid();
	}

	if (className == "Decal")
	{
		return new RBX::Decal();
	}

	if (className == "Texture")
	{
		return new RBX::Texture();
	}

	if (className == "Script" ||
		className == "BaseScript")
	{
		return new RBX::BaseScript();
	}

	if (className == "HopperBin")
	{
		return new RBX::HopperBin();
	}

	if (className == "Lighting")
	{
		return RBX::Lighting::get();
	}

	if (className == "Camera")
	{
		return RBX::Camera::get();
	}

	if (className == "Workspace" ||
		className == "PVWorkspace")
	{
		return RBX::Workspace::get();
	}

	return 0;
}
