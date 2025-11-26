#include "humanoid.h"

#include "../../../Kernel/jointsservice.h"
#include "../../World/sounds.h"
#include "../../Network/Player/players.h"
#include "../../World/ray.h"

RBX::Sound* uuhhh = RBX::Sound::fromFile("rbxasset://sounds\\uuhhh.wav");

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Humanoid>("Humanoid")
		.constructor<>()
		.property("Health", &RBX::Humanoid::health)(rttr::metadata("Type", RBX::Game))
		.property("health", &RBX::Humanoid::health)(rttr::metadata("Type", RBX::Game));
}

bool RBX::Humanoid::isValid()
{
	if (humanoidHead && humanoidRootPart)
	{
		JointsService* jointsService = JointsService::get();
		return jointsService->areConnectedPrimitives(humanoidHead->getPrimitive(), humanoidRootPart->getPrimitive());
	}
	return false;
}

void RBX::Humanoid::fetchLimbs()
{
	ModelInstance* character = getCharacter();
	if (character)
	{
		if (!humanoidHead)
		{
			humanoidHead = character->findFirstChild<PartInstance>("Head");
		}
		if (!humanoidRootPart)
		{
			humanoidRootPart = character->findFirstChild<PartInstance>("Torso");
		}
	}
}

RBX::ModelInstance* RBX::Humanoid::getCharacter()
{
	return RBXCast<RBX::ModelInstance>(getParent());
}

void RBX::Humanoid::Snap(PartInstance* p0, PartInstance* p1)
{
	JointsService* jointsService = JointsService::get();
	if (p0 && p1)
	{
		jointsService->snap(p0, p1);
	}
}

void RBX::Humanoid::editMass()
{
	Body* body = humanoidHead->getBody();
	if (body && body->body)
	{
		CoordinateFrame cofm = humanoidRootPart->getCoordinateFrame();
		dMass m;

		float dRotation[12] = toDMatrix3(cofm.rotation);

		m = body->getMass();

		m.translate(cofm.translation.x, cofm.translation.y, cofm.translation.z);
		m.rotate(dRotation);
		m.adjust(0.1f);

		body->modifyMass(m);
	}
}
void RBX::Humanoid::applyHipHeight()
{
	Body* body = humanoidHead->getBody();

	if (isJumping) {
		return;
	}

	if (body && body->body)
	{
		float distance;
		Vector3 hit, normal;
		getHumanoidGroundData(distance, hit, normal);

		Vector3 position = humanoidRootPart->getPosition();
		Vector3 velocity = body->pv->velocity.linear;

		Vector3 desired = Vector3(position.x, hit.y + hipHeight, position.z);

		if (body->getFloatMass() > 1.5f)
		{
			editMass();
		}

		if (distance <= hipHeight)
		{
			body->applyForce(Vector3(0, -velocity.y, 0));
			body->modifyPosition(desired);
		}
	}
}

void RBX::Humanoid::onStep()
{
	fetchLimbs();

	if (isValid())
	{
		RBX::StandardOut::print(RBX::MESSAGE_INFO, "uh");
		applyHipHeight();
	}
	else
	{
		ModelInstance* character = getCharacter();
		if (character)
		{
			character->buildJoints();
		}
		/* snap head seperately */
		Snap(humanoidHead, humanoidRootPart);
	}
}

void RBX::Humanoid::onKernelStep()
{

}

void RBX::Humanoid::renderAdornee(RenderDevice* renderDevice)
{
	renderMultiplayer(renderDevice);
}

void RBX::Humanoid::renderMultiplayer(RenderDevice* renderDevice)
{
	GFontRef fnt;
	RBX::Network::Player* localPlayer;

	fnt = Gui::get()->font;
	localPlayer = RBX::Network::Players::get()->localPlayer;

	if (isValid())
	{

		if (localPlayer && localPlayer->character == getParent())
		{
			return;
		}

		if (!fnt.isNull())
			/*
			* thanks
			https://github.com/Vulpovile/Blocks3D/blob/a75f7bf381c6714b8c55bb2e72acfb9077c29a08/src/source/DataModelV2/PartInstance.cpp#L114
			*/
		{
			Vector3 position = humanoidHead->getPosition();
			Vector3 gamepoint = position + Vector3(0, 1.5, 0);
			Vector3 camerapoint = renderDevice->getCameraToWorldMatrix().translation;
			float distance = pow(pow((double)gamepoint.x - (double)camerapoint.x, 2) + pow((double)gamepoint.y - (double)camerapoint.y, 2) + pow((double)gamepoint.z - (double)camerapoint.z, 2), 0.5);
			if (distance < 100 && distance > -100)
			{
				if (distance < 0)
					distance = distance * -1;

				renderDevice->pushState();

				renderDevice->setDepthWrite(0);
				renderDevice->setDepthTest((RenderDevice::DepthTest)(RenderDevice::DEPTH_ALWAYS_PASS));

				CoordinateFrame pos(renderDevice->getCameraToWorldMatrix().rotation, gamepoint);

				fnt->draw3D(renderDevice, getParent()->name, pos, 0.03 * distance, Color3::white(), Color3::black(), G3D::GFont::XALIGN_CENTER, G3D::GFont::YALIGN_CENTER);
				drawHealthbar(renderDevice, pos, distance);

				renderDevice->setDepthWrite(1);

				renderDevice->popState();

			}
		}
	}
}

void RBX::Humanoid::drawHealthbar(RenderDevice* rd, CoordinateFrame center, float distance)
{
	float f0 = (distance * 4);

	float width = f0 * (health / maxHealth) * 0.032f;
	float height = distance * 0.015f;

	float x = (distance) * -0.065f;
	float y = (distance) * -0.04f;

	Rect2D green, red;

	rd->setObjectToWorldMatrix(center);

	green = Rect2D::xywh(Vector2(x, y), Vector2(width, height));
	red = Rect2D::xywh(Vector2(x, y), Vector2(f0 * 0.032f, height));

	Draw::rect2D(red, rd, Color3::red());
	Draw::rect2D(green, rd, Color3(0.50588238f, 0.77254903f, 0.086274512f));
}

void RBX::Humanoid::setLocalTransparency(float transparency)
{
	ModelInstance* character = getCharacter();

	for (size_t i = 0; i < character->children->size(); i++)
	{
		PartInstance* pv = toInstance<PartInstance>(character->children->at(i));
		if (pv)
		{
			float fauxTransparency = pv->fauxTransparency;
			pv->setTransparency(transparency);
			pv->fauxTransparency = fauxTransparency;
		}
	}
}

void RBX::Humanoid::setWalkDirection(Vector3 walkDir)
{
	double x, z;
	double v4;

	if (walkDir != walkDirection)
	{
		if (walkDir == G3D::Vector3::zero())
		{
			walkDirection = walkDir;
			walkMode = DIRECTION_STAY;
		}
		else
		{
			x = walkDir.x;
			z = walkDir.z;

			v4 = 1.0 / sqrt(z * z + x * x);
			walkDirection.x = x * v4;
			walkDirection.y = 0;
			walkDirection.z = z * v4;
			walkDirection *= walkSpeed; /* walk speed */
			walkMode = DIRECTION_MOVE;
		}
	}
}

void RBX::Humanoid::getHumanoidGroundData(float& distanceFrom, Vector3& hit, Vector3& normal)
{
	CoordinateFrame cframe = humanoidRootPart->getCoordinateFrame();

	Ray ray = Ray::fromOriginAndDirection(cframe.translation, -Vector3::unitY());

	RBX::ISelectable* maybeGround = RBX::World::getPartFromG3DRay<Instance>(ray, hit, true, *getParent()->getChildren());

	hit = hit;
	normal = normalize(hit);
	distanceFrom = (ray.origin - hit).magnitude();
}

RBX::Humanoid* RBX::Humanoid::modelIsCharacter(RBX::Instance* testModel)
{
	if (testModel)
	{
		return testModel->findFirstChildOfClass<RBX::Humanoid>("Humanoid");
	}
	else
	{
		return 0;
	}
}

RBX::Humanoid::Humanoid()
{
	health = 100.0f;
	maxHealth = 100.0f;
	hipHeight = 4.20f;
	walkSpeed = 6;
	setClassName("Humanoid");
	setName("Humanoid");
	humanoidRootPart = 0;
	humanoidHead = 0;
}

RBX::Humanoid::~Humanoid()
{

}
