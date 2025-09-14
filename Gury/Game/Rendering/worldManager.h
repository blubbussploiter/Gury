#pragma once

#include "../Gury/Game/Rendering/renderMesh.h"

namespace RBX
{
	namespace Render
	{
		class WorldManager
		{
		private:
			bool dirty;
		public:

			void step();

			void makeDirty();
	
			static WorldManager* get();
			
		};
	}
}