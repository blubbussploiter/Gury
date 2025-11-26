#pragma once

#include "../Gury/Game/World/sounds.h"
#include "service.h"

namespace RBX
{
	class SoundService :
		public RBX::Instance, public RBX::Service<SoundService>
	{
	public:
		FMOD::System* mpSystem;

		void init();
		void update();

		static SoundService* get();

		SoundService()
		{
			name = "SoundService";
			className = "SoundService";
		}
	};

}