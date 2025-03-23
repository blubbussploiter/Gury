#include <math.h>

#include "../Gury/Game/Network/Player/players.h"
#include "../Gury/Game/World/camera.h"

#include "../Gury/Game/Services/service.h"
#include "../Gury/Game/World/sounds.h"

#include "../Gury/Game/Objects/Humanoid/humanoid.h"

void RBX::Network::PlayerController::doMove()
{
	RBX::ModelInstance* character = player->character;
	RBX::Humanoid* humanoid;
	
	if (character)
	{
		humanoid = character->findFirstChildOfClass<Humanoid>("Humanoid");

		if (!moving())
		{
			humanoid->setWalkDirection(Vector3::zero());
			return;
		}
			
		CoordinateFrame cameraCoord = Camera::get()->cframe;
		Vector3 movementHorizontal, movementVertical;
		
		movementHorizontal = cameraCoord.rightVector() * horizontal;
		movementVertical = cameraCoord.lookVector() * vertical;
		
		humanoid->setWalkDirection(normalize(movementHorizontal + movementVertical));
	}
}