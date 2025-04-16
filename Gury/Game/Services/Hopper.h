#pragma once

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/service.h"

#include "backpack.h"

namespace RBX
{
	class Hopper : public RBX::Instance, public Service<Hopper> {
	private:
		Table<HopperBin*, BackpackItem*> items;
	public:

		void updateBackpackItemGrid();
		void updateToBackpack(HopperBin* originalHopperBin, HopperBin* clonedHopperBin);

		void createFor(HopperBin* hopperBin);
		void removeFor(HopperBin* hopperBin);

		void clear();

		void doUserInput(UserInput* userInput);

		BackpackItem* createBackpackItem(HopperBin* item);
		BackpackItem* getBackpackItem(HopperBin* b) {
			if (items.containsKey(b)) {
				return items[b];
			}
			return 0;
		}

		BackpackItem* getBackpackItemByIndex(int backpackIndex);

		bool hasBin(HopperBin* hopperBin) {
			return items.containsKey(hopperBin);
		}

		static void doOnChildAdded(Instance* hopperBin);
		static void doOnChildRemoved(Instance* hopperBin);

		static void onPlayerAdded(Instance* Player);

		static Hopper* get();

		Hopper();

		RTTR_ENABLE(Instance);
	};
}