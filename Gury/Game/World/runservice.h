#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Services/service.h"

#include "../Gury/Game/Services/scriptcontext.h"

#define BODY_MASS 1.333f

namespace RBX
{
	class PhysBody; class PVInstance;
}

namespace RBX
{

	class RunService :
		public RBX::Instance,
		public RBX::Service<RunService>
	{
	private:
		RBX::Instances* steppers;
	public:

		ScriptContext* scriptContext;

		boost::signal<void()> onPause;
		boost::signal<void()> onResume;
		boost::signal<void()> onReset;

		float deltaTime;

		bool isRunning;
		bool isPaused;

		bool hasStarted; /* qnd fix for an error */
		bool shouldReset; /* queued for after collision steps */

		void run();
		void pause();
		void stop();

		void reset();
		void update();

		void heartbeat(float deltaTime);
		void updateSteppers();

		/* called after reset and before run, sets all pv's pv position back to its starting position, body position set when restartpvs called */
		void resetPvs();    

		void onWorkspaceDescendentAdded(RBX::Instance* descendent);

		static RunService* get();

		RunService()
		{
			setClassName("RunService");
			setName("Run Service");
			steppers = new RBX::Instances();
			isRunning = 0;
		}
		RTTR_ENABLE(RBX::Instance);
	};

}
