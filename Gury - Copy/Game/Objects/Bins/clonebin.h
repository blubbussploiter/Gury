#pragma once

#include "hopperbin.h"
#include "../Gury/Game/Services/backpack.h"

namespace RBX
{
	class CloneBin : public RBX::HopperBin
	{
	public:
		void update(RenderDevice* rd, G3D::UserInput* ui);
		void deactivate();
		void activate();
		CloneBin()
		{
			binType = CLONE_BIN;
		}

		virtual ~CloneBin(){deactivate();}

	};
}
