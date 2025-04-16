
#include "../Gury/Game/Gui/GuiRoot.h"

#include "backpack.h"
#include "Hopper.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Backpack>("Backpack")
		.constructor<>();
}

void RBX::Backpack::doOnChildAdded(Instance* child)
{
	RBX::HopperBin* bin = toInstance<RBX::HopperBin>(child);

	if (bin)
	{
		Hopper::get()->createFor(bin);
	}
}

void RBX::Backpack::doOnChildRemoved(Instance* child)
{
	RBX::HopperBin* bin = toInstance<RBX::HopperBin>(child);

	if (bin)
	{
		Hopper::get()->removeFor(bin);
		Hopper::get()->updateBackpackItemGrid();
	}
}