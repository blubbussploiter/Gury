#pragma once

#include "../Gury/Game/Rendering/renderMesh.h"

namespace RBX
{
	namespace Render
	{
		class WorldManager
		{
			friend class TextureReserve;
		private:
			bool dirty;
		public:

			void step();

			void makeDirty();
	
			static WorldManager* get();
			
		};
	}
}