
#include "part.h"
#include "mesh.h"

#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Services/stdout.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::PartInstance>("Part")
		 .constructor<>()
		 .property("BrickColor", &RBX::PartInstance::getBrickColor, &RBX::PartInstance::setBrickColor);
}

void RBX::PartInstance::setBrickColor(int bc)
{
	brickColor = BrickColor::getBrickMap()->brickColorFromNumber(bc);
	setColor(brickColor.color);
}

void RBX::PartInstance::write()
{
	PVInstance::write();
	writeDecals();

}

void RBX::PartInstance::edit()
{
	PVInstance::edit();
}

void RBX::PartInstance::writeDecals()
{
}
