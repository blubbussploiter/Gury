
#include "hopperbin.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::HopperBin>("HopperBin")
		.constructor<>()
		.property("BinType", &RBX::HopperBin::getBinType, &RBX::HopperBin::setBinType)(rttr::metadata("Type", RBX::Data))
		.property("TextureId", &RBX::HopperBin::getTextureId, &RBX::HopperBin::setTextureId)(rttr::metadata("Type", RBX::Data));

rttr::registration::enumeration<RBX::BinType>("BinType")
(
	rttr::value("Script", RBX::SCRIPT_BIN),
	rttr::value("GameTool", RBX::GAMETOOL_BIN),
	rttr::value("Grab", RBX::GRAB_BIN),
	rttr::value("Clone", RBX::CLONE_BIN),
	rttr::value("Hammer", RBX::HAMMER_BIN),
	rttr::value("Slingshot", RBX::SLINGSHOT_BIN),
	rttr::value("Rocket", RBX::ROCKET_BIN),
	rttr::value("Laser", RBX::LASER_BIN)
	);
}