
#pragma once

#include "../Gury/Kernel/ISteppable.h"
#include "../Gury/Game/Objects/part.h"
#include "../Gury/Game/Objects/model.h"

#include <memory>
#include <G3DAll.h>

namespace RBX
{

	enum HumanoidStates
	{
		Running,
		Strafing,
		Jumping,
		Falling,
		Tripped,
		Landed
	};

	class Humanoid : public RBX::ISteppable,
		public RBX::Render::IRenderable
	{
	private:

		enum WalkMode
		{
			DIRECTION_JUMP,
			DIRECTION_MOVE,
			DIRECTION_STAY
		};

		Vector3 walkDirection;

		WalkMode walkMode;
		HumanoidStates humanoidState;

		float hipHeight;
		float walkSpeed;

		bool isJumping;

	public:

		PartInstance* humanoidRootPart;
		PartInstance* humanoidHead;

		float health;
		float maxHealth;

		ModelInstance* getCharacter();

		bool isValid();

		void editMass();

		void applyHipHeight();
		void fetchLimbs(); /* Call every step, if one limb disappears or something.. then get it again */

		void Snap(PartInstance* p0, PartInstance* p1);

		void onStep();
		void onKernelStep();

		void renderAdornee(RenderDevice* renderDevice);
		void renderMultiplayer(RenderDevice* rd);
		void drawHealthbar(RenderDevice* rd, CoordinateFrame center, float distance);

		void setLocalTransparency(float transparency);
		void setWalkDirection(Vector3 walkDir);

		void getHumanoidGroundData(float& distanceFrom, Vector3& hit, Vector3& normal);

		static Humanoid* modelIsCharacter(RBX::Instance* testModel);

		Humanoid();
		~Humanoid();

		RBX_CLONE_DEF(Humanoid)
			RTTR_ENABLE(RBX::Render::IRenderable);
	};
}
