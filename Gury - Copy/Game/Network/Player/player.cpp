
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Objects/part.h"

#include "../Gury/Game/Objects/mesh.h"

#include "../Gury/Game/Objects/Humanoid/humanoid.h"

#include "../Gury/Game/Serializer/serializer.h"
#include "../Gury/Game/Network/replicator.h"

#include "../Gury/Game/World/workspace.h"

#include "players.h"

using namespace RBX::Network;


RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Network::Player>("Player")
		.constructor<>()
		.property("userId", &RBX::Network::Player::getUserId, &RBX::Network::Player::setUserId)(rttr::metadata("Type", RBX::Data))
		.property("AdminMode", &RBX::Network::Player::getAdminMode, &RBX::Network::Player::setAdminMode)(rttr::metadata("Type", RBX::Data))
		.property_readonly("Character", &RBX::Network::Player::getCharacter)
		.property_readonly("Backpack", &RBX::Network::Player::getBackpack)
		.method("loadCharacter", &RBX::Network::Player::loadCharacter)
		.method("LoadCharacter", &RBX::Network::Player::loadCharacter);
}

void Player::loadCharacter()
{
	RBX::Instances* load;
	RBX::Humanoid* h = 0;

	if (character)
	{
		character->remove();
	}

	load = RBX::Serializer::loadInstances(GetFileInPath("/content/font/character.rbxm"));

	if (load && load->size() > 0)
	{
		RBX::ModelInstance* possibleCharacter = toInstance<ModelInstance>(load->at(0));
		if (possibleCharacter)
		{
			h = RBX::Humanoid::modelIsCharacter(possibleCharacter);
			if (h)
			{
				character = (RBX::ModelInstance*)possibleCharacter;
			}
		}
	}

	if (!character) return;

	RBX::PartInstance* head, * humanoidRootPart;

	head = (RBX::PartInstance*)character->findFirstChild("Head");
	humanoidRootPart = (RBX::PartInstance*)character->findFirstChild("Torso");

	if (!head || !humanoidRootPart) return;

	if (!IsA<RBX::PartInstance>(head) || !IsA<RBX::PartInstance>(humanoidRootPart)) return;

	Extents levelExtents;
	Vector3 levelExtentsCentre;

	Workspace* workspace;

	workspace = Workspace::get();
	levelExtents = workspace->getModelWorldExtents();

	levelExtentsCentre = Camera::get()->camera->getCoordinateFrame().translation;
	character->translate(levelExtentsCentre);

	controller = new PlayerController();
	controller->init(this);

	Camera::get()->cameraSubject = head;
	Camera::get()->cameraType = Follow;

	character->setName(getName());
	character->setParent(workspace);

	if (RBX::RunService::get()->isRunning)
	{
		setAsController();
	}

}

void Player::disposeActiveBin()
{
	if (!activeBin)
		return;

	activeBin->remove();
	activeBin = 0;
}

void Player::setAsController()
{
	if (character)
	{
		RBX::ControllerService::get()->addController(controller);
		RBX::Camera::get()->disable(true);
	}
}

void RBX::Network::Player::PlayerRemoveFromGui()
{
	Players* players = Players::get();
	Camera* camera = Camera::get();

	if (players->getNumPlayers() == 0) {
		players->playerList->visible = false;
	}

	if (guiName) {
		guiName->remove();
	}

	if (character) {

		character->remove();

		camera->disable(false);
		camera->cameraSubject = 0;
		camera->cameraType = Fixed;
	}

}

void RBX::Network::Player::onPlayerMessageAdded(Instance* _this, Instance* playerMsg)
{
	if (IsA<Render::IRenderable>(playerMsg)) {
		WorldScene::get()->onWorkspaceDescendentAdded(toInstance<Render::IRenderable>(playerMsg));
	}
}

void RBX::Network::Player::onPlayerMessageRemoved(Instance* _this, Instance* playerMsg)
{
	if (IsA<Render::IRenderable>(playerMsg)) {
		WorldScene::get()->onWorkspaceDescendentRemoved(toInstance<Render::IRenderable>(playerMsg));
	}
}

RBX::Network::Player::~Player()
{
	PlayerRemoveFromGui();
	Players::get()->destroyPlayer(this);
}