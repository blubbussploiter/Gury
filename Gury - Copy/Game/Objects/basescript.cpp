#include "basescript.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::BaseScript>("Script")
		.constructor<>()
		.property("Source", &RBX::BaseScript::getSourceAsString, &RBX::BaseScript::setSourceFromString)(rttr::metadata("Type", RBX::Data));
}