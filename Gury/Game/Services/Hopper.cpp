
#include "../Gury/Game/World/datamodel.h"
#include "../Gury/Game/Network/Player/players.h"

#include "Hopper.h"

void RBX::Hopper::updateBackpackItemGrid()
{
	Array<HopperBin*> backpackItemBins = items.getKeys();

	for (int i = 0; i < backpackItemBins.size(); i++)
	{
		RBX::HopperBin* bin = toInstance<RBX::HopperBin>(backpackItemBins[i]);
		if (bin) {
			BackpackItem* item = items.get(bin);
			item->doUpdateGridPosition(i);
		}
	}
}

void RBX::Hopper::updateToBackpack(HopperBin* originalHopperBin, HopperBin* clonedHopperBin)
{
	if (items.containsKey(originalHopperBin)) {

		BackpackItem* backpackItem = items.get(originalHopperBin);
		items.remove(originalHopperBin);

		if (backpackItem && clonedHopperBin) {
			items.set(clonedHopperBin, backpackItem);
		}

	}

}

void RBX::Hopper::createFor(HopperBin* hopperBin)
{
	if (!items.containsKey(hopperBin))
	{
		items.set(hopperBin, createBackpackItem(hopperBin));
		RBX::Gui::get()->add(items[hopperBin]);
	}
	updateBackpackItemGrid();
}

void RBX::Hopper::removeFor(HopperBin* hopperBin)
{
	if (items.containsKey(hopperBin))
	{
		items[hopperBin]->remove();
		items.remove(hopperBin);
	}
	updateBackpackItemGrid();
}

void RBX::Hopper::clear()
{
	Array<HopperBin*> backpackItemBins = items.getKeys();

	for (int i = 0; i < backpackItemBins.size(); i++)
	{
		RBX::HopperBin* bin = toInstance<RBX::HopperBin>(backpackItemBins[i]);
		if (bin)
		{
			BackpackItem* item = items.get(bin);
			item->remove();
		}
	}

	items.clear();
}

void RBX::Hopper::doUserInput(UserInput* userInput)
{
	Array<uint16> keys;
	int index = -1;

	if (userInput->anyKeyPressed()) {

		userInput->pressedKeys(keys);
		for (int i = 0; i < keys.size(); i++) {

			uint16 key = keys[i];

			if (key >= SDLK_1 && key <= SDLK_9) {
				index = key - SDLK_1;
			}
		}

		BackpackItem* item = getBackpackItemByIndex(index);
		if (item) {
			item->doActivate();
		}
	}
}

RBX::BackpackItem* RBX::Hopper::createBackpackItem(HopperBin* item)
{
	RBX::BackpackItem* n = new RBX::BackpackItem();
	n->fromHopperBin(item);
	return n;
}

RBX::BackpackItem* RBX::Hopper::getBackpackItemByIndex(int backpackIndex) {
	Array<HopperBin*> keys = items.getKeys();
	for (int i = 0; i < keys.size(); i++) {
		BackpackItem* item = items.get(keys[i]);
		if (item->backpackIndex == backpackIndex) {
			return item;
		}
	}
	return 0;
}

void RBX::Hopper::doOnChildAdded(Instance* hopperBin)
{
	RBX::HopperBin* bin = toInstance<RBX::HopperBin>(hopperBin);

	if (bin)
	{
		Hopper::get()->createFor(bin);
		Hopper::get()->updateBackpackItemGrid();
	}
}

void RBX::Hopper::doOnChildRemoved(Instance* hopperBin)
{
	RBX::HopperBin* bin = toInstance<RBX::HopperBin>(hopperBin);

	if (bin)
	{
		if (Hopper::get()->hasBin(bin)) {
			Hopper::get()->removeFor(bin);
			Hopper::get()->updateBackpackItemGrid();
		}
	}
}

void RBX::Hopper::onPlayerAdded(Instance* Player)
{
	Network::Player* player = toInstance<Network::Player>(Player);
	if (player)
	{
		Hopper* robloxHopper = Datamodel::get()->hopper;

		if (robloxHopper) {

			for (int i = 0; i < robloxHopper->children->size(); i++)
			{
				HopperBin* hopper = toInstance< HopperBin>(robloxHopper->children->at(i));

				if (hopper) {

					HopperBin* clone = toInstance< HopperBin>(hopper->clone());
					Hopper::get()->updateToBackpack(hopper, clone);

					clone->setParent(player->backpack);


				}
			}
		}
	}
	Hopper::get()->updateBackpackItemGrid();
}

RBX::Hopper* RBX::Hopper::get()
{
	return Datamodel::get()->hopper;
}

RBX::Hopper::Hopper()
{
	name = "Hopper";
	className = "Hopper";
	
	onChildAdded.connect(doOnChildAdded);
	onChildRemoved.connect(doOnChildRemoved);

	Datamodel::get()->players->onPlayerAdded.connect(onPlayerAdded);
}
