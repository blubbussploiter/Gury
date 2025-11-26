#include "classes.h"

#include "../Gury/Game/Objects/part.h"
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Services/Hopper.h"
#include "../Gury/Game/World/sounds.h"
#include "../Gury/Game/Objects/Humanoid/humanoid.h"
#include "../Gury/Game/Objects/basescript.h"
#include "../Gury/Game/Objects/Bins/hopperbin.h"
#include "../Gury/Game/Objects/decal.h"
#include "../Gury/Game/Rendering/texture.h"
#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Objects/mesh.h"
#include "../Gury/Game/Objects/World/message.h"
#include "../Gury/Game/Objects/World/hint.h"

RBX::Instance* RBX::fromName(std::string className)
{
	/* replace with a template, or something */

	if (className == "Message") {
		return new Message();
	}

	if (className == "Hint") {
		return new Hint();
	}

	if (className == "Part")
	{
		return new RBX::PartInstance();
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
	
	if (className == "Sound")
	{
		return new RBX::Sound();
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

	if (className == "StarterPack" || className == "Hopper") {
		return RBX::Hopper::get();
	}

	if (className == "Workspace" ||
		className == "PVWorkspace")
	{
		return RBX::Workspace::get();
	}

	return 0;
}
