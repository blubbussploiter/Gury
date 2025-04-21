
#pragma once

#include "../Gury/Game/Gui/GuiRoot.h"

#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Objects/controller.h"

#include "../Gury/Game/Services/backpack.h"
#include "../Gury/Game/World/runservice.h"

namespace RBX
{
	namespace Network
	{
		class Players;
		class Player;

		class Players :
			public Instance,
			public RBX::Service<Players>
		{
		private:

			int maxPlayers;

		public:

			RBX::Gui::GuiList* playerList;

			boost::signal<void(Instance*, Player*)> onPlayerAdded;
			boost::signal<void(Instance*, Player*)> onPlayerRemoving;

			Player* localPlayer;
			Player* getLocalPlayer() { return localPlayer; }

			Player* createLocalPlayer(int userId);

			static void onPlayerNameChanged(Instance* player, std::string propertyChanged);

			void setPlayerList(RBX::Gui::GuiList* playerList);

			void destroyPlayer(Player* plr);
			void addPlayer(Player* plr);

			Color3 getPlayerColor(Player* player);

			int getNumPlayers()
			{
				int num = 0;
				for (size_t i = 0; i < children->size(); i++) {
					if (IsA<Player>(children->at(i))) {
						num++;
					}
				}
				return num;
			}

			void setMaxPlayers(int newMaxPlayers) 
			{ 
				maxPlayers = newMaxPlayers;
				onChanged(this, "maxPlayers");
			}
			int getMaxPlayers()
			{
				return maxPlayers; 
			}

			static void doOnChildAdded(Instance* _this, Instance* child);
			static void doOnChildRemoved(Instance* _this, Instance* child);

			/* unlike actual function, no arguments, instead dependant on there being a global Players class */

			static RBX::ModelInstance* findLocalCharacter();
			static Players* get();

			Players()
			{
				setClassName("Players");
				setName("Players");

				onChildAdded.connect(doOnChildAdded);
				onChildRemoved.connect(doOnChildRemoved);

				isParentLocked = 1;

				maxPlayers = 10;
				localPlayer = 0;
			}

			RTTR_ENABLE(RBX::Instance)
		};

		class PlayerController : public RBX::Controller
		{
		private:
			Player* player;
		public:

			void doMove();

			void init(Player* _plr) 
			{
				if (!player)
				{
					player = _plr;
				}
			}
		};

		class Player : 
			public RBX::Instance
		{
		private:
			RBX::Gui::GuiLabel* guiName; /* player name in list */
			PlayerController* controller;
		public:

			int userId;
			bool adminMode;

			RBX::Backpack* backpack;
			RBX::HopperBin* activeBin;

			RBX::ModelInstance* character;

			RBX::Gui::GuiLabel* getGuiName() { return guiName; }
			void setGuiName(RBX::Gui::GuiLabel* lbl) { guiName = lbl; }

			RBX::Backpack* getBackpack() { return backpack; }
			RBX::ModelInstance* getCharacter() { return character; }

			int getUserId() { return userId; }
			void setUserId(int newUserId) { userId = newUserId; }

			bool getAdminMode() { return adminMode; }
			void setAdminMode(bool newAdminMode) { adminMode = newAdminMode; }

			void loadCharacter();
			void disposeActiveBin();

			void onRemove();

			void setAsController();

			static void onPlayerMessageAdded(Instance* _this, Instance* playerMsg);
			static void onPlayerMessageRemoved(Instance* _this, Instance* playerMsg);

			Player()
			{
				setClassName("Player");
				setName("Player");

				setParent(Players::get());

				backpack = new RBX::Backpack();

				backpack->setParent(this);
				backpack->isParentLocked = true;

				onDescendentAdded.connect(onPlayerMessageAdded);
				onDescendentRemoved.connect(onPlayerMessageRemoved);

				activeBin = 0;
				controller = 0;

				character = 0;
				guiName = 0;

				userId = 0;
			}
			~Player()
			{
				Players::get()->destroyPlayer(this);
			}
			RTTR_ENABLE(RBX::Instance)
		};

	}
}