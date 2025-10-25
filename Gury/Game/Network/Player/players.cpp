
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Objects/part.h"

#include "../Gury/Game/Objects/mesh.h"

#include "../Gury/Game/Objects/Humanoid/humanoid.h"

#include "../Gury/Game/Serializer/serializer.h"
#include "../Gury/Game/Network/replicator.h"

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Services/selection.h"

#include "players.h"

using namespace RBX::Network;

/* from https://robloxopolis.com/what-colors-does-the-roblox-player-list-use/ */

static Color3 playerList_Colors[] = {
	Color3(0.67f, 0.13f, 0.13f), /* Red */
	Color3(0.16f, 0.29f, 0.84f), /* Blue */
	Color3(0.11f, 0.41f, 0.07f), /* Green */
	Color3(0.50f, 0.14f, 0.75f), /* Purple */
	Color3(1, 0.57f, 0.20f), /* Orange */
	Color3(1, 0.93f, 0.20f), /* Yellow */
	Color3(1, 0.80f, 0.95f), /* Pink */
	Color3(0.91f, 0.87f, 0.73f) /* Tan */
};

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Network::Players>("Players")
		.constructor<>()
		.property_readonly("LocalPlayer", &RBX::Network::Players::getLocalPlayer)(rttr::metadata("Nonserializable", 0))
		.property("maxPlayers", &RBX::Network::Players::getMaxPlayers, &RBX::Network::Players::setMaxPlayers)(rttr::metadata("Type", RBX::Data))
		.property_readonly("numPlayers", &RBX::Network::Players::getNumPlayers)(rttr::metadata("Type", RBX::Data), rttr::metadata("Nonserializable", 0))
		.method("createLocalPlayer", &RBX::Network::Players::createLocalPlayer);
}

Player* Players::createLocalPlayer(int userId)
{
	Player* player;

	if (localPlayer)
	{
		throw std::runtime_error("Local player already exists");
	}

	player = new Player();
	addPlayer(player);

	if (userId == 0) {
		userId = 1;
	}

	player->userId = userId;
	player->setName("Player");

	localPlayer = player;
	SelectionService::get()->selectionAllowed = false;

	return localPlayer;
}

void RBX::Network::Players::onPlayerNameChanged(Instance* plr, rttr::property propertyChanged)
{
	std::string name = propertyChanged.get_name().to_string();

	if (name == "Name") {

		Player* player = toInstance<Player>(plr);
		Gui::GuiLabel* lbl = player->getGuiName();

		if (lbl && lbl->title != player->getName())
		{
			lbl->title = player->getName();
			lbl->textColor = Players::get()->getPlayerColor(player);
		}

	}
}

void RBX::Network::Players::setPlayerList(RBX::Gui::GuiList* _playerList)
{
	playerList = _playerList;
}

void Players::destroyPlayer(Player* plr)
{
	plr->remove();
}

void Players::addPlayer(Player* player)
{
	RBX::Gui::GuiLabel* guiName;

	guiName = new RBX::Gui::GuiLabel();

	guiName->textColor = getPlayerColor(player);
	guiName->title = player->getName();
	guiName->sz = 10;

	player->setParent(this);
	player->setGuiName(guiName);
	player->onChanged.connect(onPlayerNameChanged);

	playerList->visible = true;
	playerList->addChild(guiName);
}

Color3 RBX::Network::Players::getPlayerColor(Player* player)
{
	return playerList_Colors[rand() % sizeof(playerList_Colors) / sizeof(Color3)];
}

void RBX::Network::Players::doOnChildAdded(Instance* _this, Instance* child)
{
	Player* player = toInstance<Player>(child);
	if (player)
	{
		Players::get()->onPlayerAdded(_this, player);
	}
}

void RBX::Network::Players::doOnChildRemoved(Instance* _this, Instance* child)
{
	Player* player = toInstance<Player>(child);
	if (player)
	{
		player->PlayerRemoveFromGui();
		Players::get()->onPlayerRemoving(_this, player);
	}
}

RBX::ModelInstance* Players::findLocalCharacter()
{
	Players* players = Players::get();
	Player* player;

	player = players->localPlayer;

	if (player && (player->character))
		return player->character;

	return 0;
}

Players* RBX::Network::Players::get()
{
	return Datamodel::get()->players;
}
